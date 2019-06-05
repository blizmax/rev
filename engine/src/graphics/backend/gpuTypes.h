//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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
#include <cassert>
#include <vector>

namespace rev::gfx
{
	enum class ScalarType : uint8_t
	{
		int8 = 1,
		uint8 = 1,
		int16 = 2,
		uint16 = 2,
		int32 = 4,
		uint32 = 4,
		float32 = 4,
	};

	struct PixelFormat
	{
		ScalarType componentType;
		std::uint8_t numChannels;
		bool sRGB = false;

		uint8_t pixelSize() const {
			return numChannels * uint8_t(componentType);
		}

		bool operator==(const PixelFormat& b) const
		{
			return componentType == b.componentType && numChannels == b.numChannels;
		}
	};

	enum class ZTest {
		None = 0,
		Lequal = 1,
		Gequal = 2
	};

	enum class Winding
	{
		CW,
		CCW
	};

	struct RasterOptions
	{
		bool cullBack = false;// Culling
		bool cullFront = false;
		Winding frontFace = Winding::CCW;
		ZTest depthTest = ZTest::None;// Depth test

		using Mask = uint32_t;

		Mask mask() const {
			Mask m = 0;
			m |= (cullBack ? 1 : 0);
			m |= (cullFront ? 1 : 0) << 1;
			m |= ((frontFace == Winding::CCW) ? 1 : 0) << 2;
			m |= (int(depthTest)) << 3;
			return m;
		}

		static RasterOptions fromMask(Mask m)
		{
			RasterOptions r;
			r.cullBack = m & 1;
			r.cullFront = m & (1 << 1);
			r.frontFace = Winding((m >> 2) & 1);
			r.depthTest = ZTest(m >> 3);
			return r;
		}
	};

	enum class DescriptorType
	{
		RenderTarget,
		DepthStencil,
		Sampler,
		ShaderResource,
	};

	class DescriptorHeap
	{};

	class RootSignature
	{
	public:
		struct Constant
		{
			uint8_t byteSize;
			uint8_t bindingPosition;
		};

		static constexpr int MAX_CONSTANTS = 32;

		struct Desc
		{
			uint32_t numConstants = 0;
			Constant constants[MAX_CONSTANTS];

			template<class T>
			void addParam(uint8_t bindingPosition)
			{
				// TODO: Support sizes under 4 bytes, adding padding at the end
				static_assert(sizeof(T) % 16 == 0);
				assert(numConstants < MAX_CONSTANTS);

				auto& constant = constants[numConstants++];
				constant.bindingPosition = bindingPosition;
				constant.byteSize = sizeof(T);
			}
		};
	};

	class GpuBuffer
	{};

	enum class RenderTargetType
	{
		Depth,
		Color
	};

	class RenderTargetView
	{
	};

	enum BufferType
	{
		Resident, // Resident on gpu memory, fastest gpu bandwidth
		Upload, // Used to upload content from the CPU to the GPU
		ReadBack // Used to read content from the GPU back into the CPU
	};

	enum ResourceFlags
	{
		None = 0,
		IsRenderTarget = 1,
		IsDepthStencil = 2,
	};

	class RasterPipeline
	{
	public:
		struct Attribute
		{
			// Source buffer
			size_t binding; // Location (in shader)
			size_t offset = 0;
			ScalarType componentType = ScalarType::float32;
			size_t numComponents = 1;
			size_t stride = 0;
			bool normalized = false;
		};

		struct Desc
		{
			RootSignature* signature;

			// Vtx stage
			int numAttributes;
			Attribute* vtxAttributes; // TODO: This can be reused later for instance data
			std::vector<std::string> vtxCode;

			// Pxl stage
			std::vector<std::string> pxlCode;
			RasterOptions raster;

			// TODO: Define render target layout
		};
	};

	class CommandPool
	{
	public:
		enum Type
		{
			Graphics,
			Compute,
			Copy,
			Bundle
		};

		virtual void reset() = 0;
	};
}
