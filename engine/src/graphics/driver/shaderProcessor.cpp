//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "shaderProcessor.h"
#include <iostream>
#include <core/platform/fileSystem/file.h>
#include <core/string_util.h>

using namespace std;

namespace rev::graphics
{
	//------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::loadCodeFromFile(const std::string& fileName, std::string& outCode, MetaData& metadata)
	{
		// load code from the entry file
		core::File baseFile(fileName);
		if(!baseFile.sizeInBytes())
			return false;

		// Init parsing context
		Context context;
		context.m_fileStack.emplace_back(fileName);
		context.m_includePathStack.push_back(core::getPathFolder(fileName));
		context.m_pendingCode.emplace_back(0, baseFile.bufferAsText());

		// Add first dependency
		metadata.dependencies.emplace_back(fileName);

		// Parse code
		return processCode(context, true, outCode, metadata);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processCode(Context& context, bool followIncludes, std::string& outCode, MetaData& metadata)
	{
		// Parse code
		while(!context.m_pendingCode.empty())
		{
			auto& [lineStart, code] = context.m_pendingCode.back();
			// Extract line
			auto lineEnd = code.find('\n', lineStart);
			bool isLastLine = (lineEnd == std::string::npos);
			if(isLastLine) // Last line
			{
				lineEnd = code.length();
			}
			// Process line
			if(lineStart < code.length())
			{
				auto success = processLine(code.substr(lineStart, lineEnd-lineStart+1), context, followIncludes, metadata, outCode);

				lineStart = lineEnd+1; // Prepare next line
				if(!success)
					return false;
			}
			// Prepare next iteration
			lineStart = lineEnd+1;
			if(isLastLine)
			{
				outCode.append("\n");
				context.m_pendingCode.pop_back();
				context.m_includePathStack.pop_back();
			}
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processLine(const std::string& line, Context& context, bool followIncludes, MetaData& metadata, std::string& outCode)
	{
		const string includeLabel = "#include";
		const string pragmaLabel = "#pragma";
		const string uniformLabel = "uniform";

		bool success = true;

		if(line.substr(0, includeLabel.length()) == includeLabel) // Include line
		{
			success = processInclude(line, context, followIncludes, metadata, outCode);
		}
		else if(line.substr(0, pragmaLabel.length()) == pragmaLabel)
		{
			string payload = line.substr(pragmaLabel.length()+1);
			metadata.pragmas.push_back(payload);
		}
		/* TODDO: Free location uniforms
		else if(line.substr(0, uniformLabel.length()) == uniformLabel) // uniforms
		{
			string payload = line.substr(uniformLabel.length()+1);
			metadata.uniforms.push_back(payload);
		}*/
		else if(line.compare(0, 6, "layout") == 0)
		{
			success = processUniform(line, metadata);
		}
		else // Regular line
		{
			outCode.append(line);
		}

		return success;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processInclude(const std::string& line, Context& context, bool followIncludes, MetaData& metadata, std::string& outCode)
	{
		const string includeLabel = "#include";

		// Find the include path
		auto startPos = line.find('"', includeLabel.length()) + 1;
		auto endPos = line.find('"', startPos);

		auto pathAppend = line.substr(startPos, endPos-startPos);
		if(pathAppend.empty())
		{
			cout << "Error parsing shader include. Empty include processing " << context.m_fileStack.back() << "\n";
			return false;
		}

		auto fullPath = context.m_includePathStack.back() + pathAppend;
		metadata.dependencies.emplace_back(fullPath);

		if(followIncludes)
		{
			core::File includedFile(fullPath);
			if(includedFile.sizeInBytes() == 0)
			{
				cout << "Error: unable to find include file " << fullPath << " while parsing shader file " << context.m_fileStack.back() << "\n";
				return false;
			}
			context.m_includePathStack.push_back(core::getPathFolder(fullPath));
			context.m_pendingCode.emplace_back(0, includedFile.bufferAsText());
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processUniform(const std::string& line, MetaData& metadata)
	{
		Uniform prop;
		// Parse uniform location, and store it in the property
		auto loc_pos = line.find_first_not_of(" \t", line.find('=')+1);
		prop.location = stoi(line.substr(loc_pos));
		auto arg_pos = line.find("sampler2D", loc_pos);
		if(arg_pos != string::npos)
			prop.type = Uniform::Texture2D;
		else {
			arg_pos = line.find("vec3", loc_pos);
			if(arg_pos != string::npos)
				prop.type = Uniform::Vec3;
			else {
				arg_pos = line.find("vec4", loc_pos);
				if(arg_pos != string::npos)
					prop.type = Uniform::Vec4;
				else {
					arg_pos = line.find("float", loc_pos);
					if(arg_pos != string::npos)
						prop.type = Uniform::Scalar;
					else
						return false;
				}
			}
		}
		arg_pos = line.find_first_of(" \t", arg_pos);
		auto name_pos = line.find_first_not_of(" \t", arg_pos);
		auto name_end = line.find_first_of(" \t;", name_pos);
		prop.name = line.substr(name_pos, name_end-name_pos);
		metadata.uniforms.push_back(prop);

		return true;
	}

}	// namespace rev::graphics