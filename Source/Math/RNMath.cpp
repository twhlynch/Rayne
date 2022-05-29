//
//  RNMath.cpp
//  Rayne
//
//  Copyright 2015 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNMath.h"
#include "RNVector.h"

namespace RN
{
	namespace Math
	{
#if RN_SIMD
		RN_ALIGNAS(128) const uint32 ITrigonometryTable[256][2] =
		{
			{0x3F800000, 0x00000000}, {0x3F7FEC43, 0x3CC90AB0}, {0x3F7FB10F, 0x3D48FB30}, {0x3F7F4E6D, 0x3D96A905}, {0x3F7EC46D, 0x3DC8BD36}, {0x3F7E1324, 0x3DFAB273}, {0x3F7D3AAC, 0x3E164083}, {0x3F7C3B28, 0x3E2F10A3},
			{0x3F7B14BE, 0x3E47C5C2}, {0x3F79C79D, 0x3E605C13}, {0x3F7853F8, 0x3E78CFCD}, {0x3F76BA07, 0x3E888E94}, {0x3F74FA0B, 0x3E94A031}, {0x3F731447, 0x3EA09AE5}, {0x3F710908, 0x3EAC7CD4}, {0x3F6ED89E, 0x3EB8442A},
			{0x3F6C835E, 0x3EC3EF16}, {0x3F6A09A6, 0x3ECF7BCB}, {0x3F676BD8, 0x3EDAE880}, {0x3F64AA59, 0x3EE63375}, {0x3F61C597, 0x3EF15AEA}, {0x3F5EBE05, 0x3EFC5D28}, {0x3F5B941A, 0x3F039C3D}, {0x3F584853, 0x3F08F59B},
			{0x3F54DB31, 0x3F0E39DA}, {0x3F514D3D, 0x3F13682B}, {0x3F4D9F02, 0x3F187FC0}, {0x3F49D112, 0x3F1D7FD2}, {0x3F45E403, 0x3F22679A}, {0x3F41D870, 0x3F273656}, {0x3F3DAEF9, 0x3F2BEB4A}, {0x3F396842, 0x3F3085BB},
			{0x3F3504F3, 0x3F3504F3}, {0x3F3085BA, 0x3F396842}, {0x3F2BEB49, 0x3F3DAEFA}, {0x3F273655, 0x3F41D871}, {0x3F226799, 0x3F45E403}, {0x3F1D7FD1, 0x3F49D112}, {0x3F187FC0, 0x3F4D9F02}, {0x3F13682A, 0x3F514D3D},
			{0x3F0E39D9, 0x3F54DB32}, {0x3F08F59B, 0x3F584853}, {0x3F039C3C, 0x3F5B941B}, {0x3EFC5D27, 0x3F5EBE05}, {0x3EF15AE7, 0x3F61C598}, {0x3EE63374, 0x3F64AA59}, {0x3EDAE881, 0x3F676BD8}, {0x3ECF7BC9, 0x3F6A09A7},
			{0x3EC3EF15, 0x3F6C835E}, {0x3EB84427, 0x3F6ED89E}, {0x3EAC7CD3, 0x3F710908}, {0x3EA09AE2, 0x3F731448}, {0x3E94A030, 0x3F74FA0B}, {0x3E888E93, 0x3F76BA07}, {0x3E78CFC8, 0x3F7853F8}, {0x3E605C12, 0x3F79C79D},
			{0x3E47C5BC, 0x3F7B14BF}, {0x3E2F10A0, 0x3F7C3B28}, {0x3E164085, 0x3F7D3AAC}, {0x3DFAB26C, 0x3F7E1324}, {0x3DC8BD35, 0x3F7EC46D}, {0x3D96A8FB, 0x3F7F4E6D}, {0x3D48FB29, 0x3F7FB10F}, {0x3CC90A7E, 0x3F7FEC43},
			{0x00000000, 0x3F800000}, {0xBCC90A7E, 0x3F7FEC43}, {0xBD48FB29, 0x3F7FB10F}, {0xBD96A8FB, 0x3F7F4E6D}, {0xBDC8BD35, 0x3F7EC46D}, {0xBDFAB26C, 0x3F7E1324}, {0xBE164085, 0x3F7D3AAC}, {0xBE2F10A0, 0x3F7C3B28},
			{0xBE47C5BC, 0x3F7B14BF}, {0xBE605C12, 0x3F79C79D}, {0xBE78CFC8, 0x3F7853F8}, {0xBE888E93, 0x3F76BA07}, {0xBE94A030, 0x3F74FA0B}, {0xBEA09AE2, 0x3F731448}, {0xBEAC7CD3, 0x3F710908}, {0xBEB84427, 0x3F6ED89E},
			{0xBEC3EF15, 0x3F6C835E}, {0xBECF7BC9, 0x3F6A09A7}, {0xBEDAE881, 0x3F676BD8}, {0xBEE63374, 0x3F64AA59}, {0xBEF15AE7, 0x3F61C598}, {0xBEFC5D27, 0x3F5EBE05}, {0xBF039C3C, 0x3F5B941B}, {0xBF08F59B, 0x3F584853},
			{0xBF0E39D9, 0x3F54DB32}, {0xBF13682A, 0x3F514D3D}, {0xBF187FC0, 0x3F4D9F02}, {0xBF1D7FD1, 0x3F49D112}, {0xBF226799, 0x3F45E403}, {0xBF273655, 0x3F41D871}, {0xBF2BEB49, 0x3F3DAEFA}, {0xBF3085BA, 0x3F396842},
			{0xBF3504F3, 0x3F3504F3}, {0xBF396842, 0x3F3085BB}, {0xBF3DAEF9, 0x3F2BEB4A}, {0xBF41D870, 0x3F273656}, {0xBF45E403, 0x3F22679A}, {0xBF49D112, 0x3F1D7FD2}, {0xBF4D9F02, 0x3F187FC0}, {0xBF514D3D, 0x3F13682B},
			{0xBF54DB31, 0x3F0E39DA}, {0xBF584853, 0x3F08F59B}, {0xBF5B941A, 0x3F039C3D}, {0xBF5EBE05, 0x3EFC5D28}, {0xBF61C597, 0x3EF15AEA}, {0xBF64AA59, 0x3EE63375}, {0xBF676BD8, 0x3EDAE880}, {0xBF6A09A6, 0x3ECF7BCB},
			{0xBF6C835E, 0x3EC3EF16}, {0xBF6ED89E, 0x3EB8442A}, {0xBF710908, 0x3EAC7CD4}, {0xBF731447, 0x3EA09AE5}, {0xBF74FA0B, 0x3E94A031}, {0xBF76BA07, 0x3E888E94}, {0xBF7853F8, 0x3E78CFCD}, {0xBF79C79D, 0x3E605C13},
			{0xBF7B14BE, 0x3E47C5C2}, {0xBF7C3B28, 0x3E2F10A3}, {0xBF7D3AAC, 0x3E164083}, {0xBF7E1324, 0x3DFAB273}, {0xBF7EC46D, 0x3DC8BD36}, {0xBF7F4E6D, 0x3D96A905}, {0xBF7FB10F, 0x3D48FB30}, {0xBF7FEC43, 0x3CC90AB0},
			{0xBF800000, 0x00000000}, {0xBF7FEC43, 0xBCC90AB0}, {0xBF7FB10F, 0xBD48FB30}, {0xBF7F4E6D, 0xBD96A905}, {0xBF7EC46D, 0xBDC8BD36}, {0xBF7E1324, 0xBDFAB273}, {0xBF7D3AAC, 0xBE164083}, {0xBF7C3B28, 0xBE2F10A3},
			{0xBF7B14BE, 0xBE47C5C2}, {0xBF79C79D, 0xBE605C13}, {0xBF7853F8, 0xBE78CFCD}, {0xBF76BA07, 0xBE888E94}, {0xBF74FA0B, 0xBE94A031}, {0xBF731447, 0xBEA09AE5}, {0xBF710908, 0xBEAC7CD4}, {0xBF6ED89E, 0xBEB8442A},
			{0xBF6C835E, 0xBEC3EF16}, {0xBF6A09A6, 0xBECF7BCB}, {0xBF676BD8, 0xBEDAE880}, {0xBF64AA59, 0xBEE63375}, {0xBF61C597, 0xBEF15AEA}, {0xBF5EBE05, 0xBEFC5D28}, {0xBF5B941A, 0xBF039C3D}, {0xBF584853, 0xBF08F59B},
			{0xBF54DB31, 0xBF0E39DA}, {0xBF514D3D, 0xBF13682B}, {0xBF4D9F02, 0xBF187FC0}, {0xBF49D112, 0xBF1D7FD2}, {0xBF45E403, 0xBF22679A}, {0xBF41D870, 0xBF273656}, {0xBF3DAEF9, 0xBF2BEB4A}, {0xBF396842, 0xBF3085BB},
			{0xBF3504F3, 0xBF3504F3}, {0xBF3085BA, 0xBF396842}, {0xBF2BEB49, 0xBF3DAEFA}, {0xBF273655, 0xBF41D871}, {0xBF226799, 0xBF45E403}, {0xBF1D7FD1, 0xBF49D112}, {0xBF187FC0, 0xBF4D9F02}, {0xBF13682A, 0xBF514D3D},
			{0xBF0E39D9, 0xBF54DB32}, {0xBF08F59B, 0xBF584853}, {0xBF039C3C, 0xBF5B941B}, {0xBEFC5D27, 0xBF5EBE05}, {0xBEF15AE7, 0xBF61C598}, {0xBEE63374, 0xBF64AA59}, {0xBEDAE881, 0xBF676BD8}, {0xBECF7BC9, 0xBF6A09A7},
			{0xBEC3EF15, 0xBF6C835E}, {0xBEB84427, 0xBF6ED89E}, {0xBEAC7CD3, 0xBF710908}, {0xBEA09AE2, 0xBF731448}, {0xBE94A030, 0xBF74FA0B}, {0xBE888E93, 0xBF76BA07}, {0xBE78CFC8, 0xBF7853F8}, {0xBE605C12, 0xBF79C79D},
			{0xBE47C5BC, 0xBF7B14BF}, {0xBE2F10A0, 0xBF7C3B28}, {0xBE164085, 0xBF7D3AAC}, {0xBDFAB26C, 0xBF7E1324}, {0xBDC8BD35, 0xBF7EC46D}, {0xBD96A8FB, 0xBF7F4E6D}, {0xBD48FB29, 0xBF7FB10F}, {0xBCC90A7E, 0xBF7FEC43},
			{0x00000000, 0xBF800000}, {0x3CC90A7E, 0xBF7FEC43}, {0x3D48FB29, 0xBF7FB10F}, {0x3D96A8FB, 0xBF7F4E6D}, {0x3DC8BD35, 0xBF7EC46D}, {0x3DFAB26C, 0xBF7E1324}, {0x3E164085, 0xBF7D3AAC}, {0x3E2F10A0, 0xBF7C3B28},
			{0x3E47C5BC, 0xBF7B14BF}, {0x3E605C12, 0xBF79C79D}, {0x3E78CFC8, 0xBF7853F8}, {0x3E888E93, 0xBF76BA07}, {0x3E94A030, 0xBF74FA0B}, {0x3EA09AE2, 0xBF731448}, {0x3EAC7CD3, 0xBF710908}, {0x3EB84427, 0xBF6ED89E},
			{0x3EC3EF15, 0xBF6C835E}, {0x3ECF7BC9, 0xBF6A09A7}, {0x3EDAE881, 0xBF676BD8}, {0x3EE63374, 0xBF64AA59}, {0x3EF15AE7, 0xBF61C598}, {0x3EFC5D27, 0xBF5EBE05}, {0x3F039C3C, 0xBF5B941B}, {0x3F08F59B, 0xBF584853},
			{0x3F0E39D9, 0xBF54DB32}, {0x3F13682A, 0xBF514D3D}, {0x3F187FC0, 0xBF4D9F02}, {0x3F1D7FD1, 0xBF49D112}, {0x3F226799, 0xBF45E403}, {0x3F273655, 0xBF41D871}, {0x3F2BEB49, 0xBF3DAEFA}, {0x3F3085BA, 0xBF396842},
			{0x3F3504F3, 0xBF3504F3}, {0x3F396842, 0xBF3085BB}, {0x3F3DAEF9, 0xBF2BEB4A}, {0x3F41D870, 0xBF273656}, {0x3F45E403, 0xBF22679A}, {0x3F49D112, 0xBF1D7FD2}, {0x3F4D9F02, 0xBF187FC0}, {0x3F514D3D, 0xBF13682B},
			{0x3F54DB31, 0xBF0E39DA}, {0x3F584853, 0xBF08F59B}, {0x3F5B941A, 0xBF039C3D}, {0x3F5EBE05, 0xBEFC5D28}, {0x3F61C597, 0xBEF15AEA}, {0x3F64AA59, 0xBEE63375}, {0x3F676BD8, 0xBEDAE880}, {0x3F6A09A6, 0xBECF7BCB},
			{0x3F6C835E, 0xBEC3EF16}, {0x3F6ED89E, 0xBEB8442A}, {0x3F710908, 0xBEAC7CD4}, {0x3F731447, 0xBEA09AE5}, {0x3F74FA0B, 0xBE94A031}, {0x3F76BA07, 0xBE888E94}, {0x3F7853F8, 0xBE78CFCD}, {0x3F79C79D, 0xBE605C13},
			{0x3F7B14BE, 0xBE47C5C2}, {0x3F7C3B28, 0xBE2F10A3}, {0x3F7D3AAC, 0xBE164083}, {0x3F7E1324, 0xBDFAB273}, {0x3F7EC46D, 0xBDC8BD36}, {0x3F7F4E6D, 0xBD96A905}, {0x3F7FB10F, 0xBD48FB30}, {0x3F7FEC43, 0xBCC90AB0}
		};
		
		static inline const Vector2 *TrigonometryTable()
		{
			return reinterpret_cast<const Vector2 *>(ITrigonometryTable);
		}
#endif
		
		
		float Sqrt(float x)
		{
#if RN_SIMD
			SIMD::StoreX(_mm_sqrt_ss(SIMD::LoadScalar(&x)), &x);
			return x;
#else
			return sqrtf(x);
#endif
		}
		
		float InverseSqrt(float x)
		{
#if RN_SIMD
			float result;
			
			SIMD::VecFloat vector = SIMD::LoadScalar(&x);
			SIMD::VecFloat mask   = SIMD::Cmplt(vector, SIMD::LoadConstant<0x800000>());
			
			SIMD::VecFloat r = _mm_rsqrt_ss(vector);
			r = _mm_mul_ss(_mm_mul_ss(_mm_sub_ss(SIMD::LoadConstant<0x40400000>(), _mm_mul_ss(vector, _mm_mul_ss(r, r))), r), SIMD::LoadConstant<0x3F000001>());
			
			SIMD::StoreX(SIMD::Select(r, SIMD::LoadConstant<0x7F800000>(), mask), &result);
			return result;
#else
			return 1.0 / sqrtf(x);
#endif
		}
		
		
		float Sin(float x)
		{
#if RN_SIMD
			float result;
			
			SIMD::VecFloat b = SIMD::MulScalar(SIMD::AndNot(SIMD::LoadScalar(&x), SIMD::NegativeZero()), SIMD::LoadConstant<0x4222F983>());
			SIMD::VecFloat i = SIMD::PositiveFloor(b);
			
			b = SIMD::MulScalar(SIMD::SubScalar(b, i), SIMD::LoadConstant<0x3cc90fdb>());
			
			const Vector2 &cossin = TrigonometryTable()[SIMD::TruncateConvert(i) & 255];
			
			SIMD::VecFloat cosine_alpha = SIMD::LoadScalar(&cossin.x);
			SIMD::VecFloat sine_alpha   = SIMD::LoadScalar(&cossin.y);
			
			SIMD::VecFloat b2 = SIMD::MulScalar(b, b);
			SIMD::VecFloat sine_beta = SIMD::NmsubScalar(SIMD::MulScalar(b, b2), SIMD::NmsubScalar(b2, SIMD::LoadConstant<0x3E2AAAAB>(), SIMD::LoadConstant<0x3C088889>()), b);
			SIMD::VecFloat cosine_beta = SIMD::NmsubScalar(b2, SIMD::Nmsub(b2, SIMD::LoadConstant<0x3D2AAAAB>(), SIMD::LoadConstant<0x3F000000>()), SIMD::LoadConstant<0x3F800000>());
			
			SIMD::VecFloat sine = SIMD::MaddScalar(sine_alpha, cosine_beta, SIMD::MulScalar(cosine_alpha, sine_beta));
			
			SIMD::StoreX((x < 0.0f) ? SIMD::Negate(sine) : sine, &result);
			
			return result;
#else
			return sinf(x);
#endif
		}
		
		float Cos(float x)
		{
#if RN_SIMD
			float result;

			SIMD::VecFloat b = SIMD::MulScalar(SIMD::AndNot(SIMD::LoadScalar(&x), SIMD::NegativeZero()), SIMD::LoadConstant<0x4222F983>());
			SIMD::VecFloat i = SIMD::PositiveFloorScalar(b);

			b = SIMD::MulScalar(SIMD::SubScalar(b, i), SIMD::LoadConstant<0x3CC90FDB>());

			const Vector2 &cossin = TrigonometryTable()[SIMD::TruncateConvert(i) & 255];
			SIMD::VecFloat cosine_alpha = SIMD::LoadScalar(&cossin.x);
			SIMD::VecFloat sine_alpha = SIMD::LoadScalar(&cossin.y);

			SIMD::VecFloat b2 = SIMD::MulScalar(b, b);
			SIMD::VecFloat sine_beta = SIMD::NmsubScalar(SIMD::MulScalar(b, b2), SIMD::NmsubScalar(b2, SIMD::LoadConstant<0x3E2AAAAB>(), SIMD::LoadConstant<0x3C088889>()), b);
			SIMD::VecFloat cosine_beta = SIMD::NmsubScalar(b2, SIMD::Nmsub(b2, SIMD::LoadConstant<0x3D2AAAAB>(), SIMD::LoadConstant<0x3F000000>()), SIMD::LoadConstant<0x3F800000>());

			SIMD::StoreX(SIMD::SubScalar(SIMD::MulScalar(cosine_alpha, cosine_beta), SIMD::MulScalar(sine_alpha, sine_beta)), &result);
			return result;
#else
			return cosf(x);
#endif
		}
	
		//Taken from https://developer.android.google.cn/games/optimize/vertex-data-management
		uint16 ConvertFloatToHalf(float value)
		{
			uint32 x = *(uint32 *)&value;
			uint32 sign = (uint16)(x >> 31);
			uint32 mantissa;
			uint32 exp;
			uint16 hf;

			mantissa = x & ((1 << 23) - 1);
			exp = x & (0xFF << 23);
			if(exp >= 0x47800000)
			{
				// check if the original number is a NaN
				if(mantissa && (exp == (0xFF << 23)))
				{
					// single precision NaN
					mantissa = (1 << 23) - 1;
				}
				else
				{
					// half-float will be Inf
					mantissa = 0;
				}
				hf = (((uint16)sign) << 15) | (uint16)(0x1F << 10) | (uint16)(mantissa >> 13);
			}
			// check if exponent is <= -15
			else if(exp <= 0x38000000)
			{
				hf = 0;  // too small to be represented
			}
			else
			{
				hf = (((uint16)sign) << 15) | (uint16)((exp - 0x38000000) >> 13) | (uint16)(mantissa >> 13);
			}

			return hf;
		}
	
		//Taken from: https://gist.github.com/milhidaka/95863906fe828198f47991c813dbe233
		float ConvertHalfToFloat(uint16 value)
		{
		  // MSB -> LSB
		  // float16=1bit: sign, 5bit: exponent, 10bit: fraction
		  // float32=1bit: sign, 8bit: exponent, 23bit: fraction
		  // for normal exponent(1 to 0x1e): value=2**(exponent-15)*(1.fraction)
		  // for denormalized exponent(0): value=2**-14*(0.fraction)
		  uint32_t sign = value >> 15;
		  uint32_t exponent = (value >> 10) & 0x1F;
		  uint32_t fraction = (value & 0x3FF);
		  uint32_t float32_value;
		  if (exponent == 0)
		  {
			if (fraction == 0)
			{
			  // zero
			  float32_value = (sign << 31);
			}
			else
			{
			  // can be represented as ordinary value in float32
			  // 2 ** -14 * 0.0101
			  // => 2 ** -16 * 1.0100
			  // int int_exponent = -14;
			  exponent = 127 - 14;
			  while ((fraction & (1 << 10)) == 0)
			  {
				//int_exponent--;
				exponent--;
				fraction <<= 1;
			  }
			  fraction &= 0x3FF;
			  // int_exponent += 127;
			  float32_value = (sign << 31) | (exponent << 23) | (fraction << 13);
			}
		  }
		  else if (exponent == 0x1F)
		  {
			/* Inf or NaN */
			float32_value = (sign << 31) | (0xFF << 23) | (fraction << 13);
		  }
		  else
		  {
			/* ordinary number */
			float32_value = (sign << 31) | ((exponent + (127-15)) << 23) | (fraction << 13);
		  }
		  
		  return *((float*)&float32_value);
		}
	}
}
