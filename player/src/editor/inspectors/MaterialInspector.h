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
#pragma once
#include "../editor.h"
#include <graphics/scene/material.h>
#include <graphics/debug/imgui.h>
#include <game/textureManager.h>
#include "../project/asset.h"
#include "assetInspector.h"

namespace rev { namespace editor {

	struct MaterialInspector : public AssetInspector
	{
		MaterialInspector(Asset& asset)
			: AssetInspector(asset)
		{}

		void customInspectionPanel() const override
		{
			ImGui::Text("Material inspector");
		}
		/*MaterialInspector(const std::vector<std::string>& _textures, game::TextureManager& _mgr)
			: mTextures(_textures)
			, mTextureMgr(_mgr)
		{}*/

		/*void customIns(graphics::Material& mat) const
		{
			std::vector<char>	nameBuffer;
			nameBuffer.resize(512);
			memcpy(nameBuffer.data(), mat.name.c_str(), mat.name.size()+1);
			if(ImGui::InputText("Name", nameBuffer.data(), nameBuffer.size()))
				mat.name = nameBuffer.data();
			// Textures
			auto albedo = mat.texture(5);
			if(albedo)
			{
				if(ImGui::BeginCombo("Albedo", albedo->name.c_str()))
				{
					for(auto& texture : mTextures)
					{
						if(ImGui::Selectable(texture.c_str(), albedo->name==texture))
							mat.setTexture(5, mTextureMgr.get(texture));
					}
					ImGui::EndCombo();
				}
			}
			// Float params
			auto roughness = mat.floatParam(6);
			if(roughness)
				ImGui::SliderFloat("Roughness", roughness, 0.f, 1.f, "%.2f");
			auto metallic = mat.floatParam(7);
			if(metallic)
				ImGui::SliderFloat("Metallic", metallic, 0.f, 1.f, "%.2f");

			if(ImGui::Button("Save"))
				mat.save();
		}*/

	private:
		//const std::vector<std::string>& mTextures;
		//game::TextureManager&			mTextureMgr;
	};

}}	// namespace rev::editor