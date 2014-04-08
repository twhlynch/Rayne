//
//  RNDebug.cpp
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNDebug.h"
#include "RNMaterial.h"
#include "RNRenderer.h"
#include "RNThread.h"
#include "RNString.h"
#include "RNWrappingObject.h"
#include "RNThreadLocalStorage.h"

namespace RN
{
	namespace Debug
	{
		struct Point2D
		{
			Point2D(const Vector2& tpoint, const Color& tcolor) :
				point(tpoint),
				color(tcolor)
			{}
			
			Vector2 point;
			Color color;
		};
		
		struct Point3D
		{
			Point3D(const Vector3& tpoint, const Color& tcolor) :
				point(tpoint),
				color(tcolor)
			{}
			
			Vector3 point;
			Color color;
		};
		
		typedef std::vector<Point2D> Line2D;
		typedef std::vector<Point3D> Line3D;
		
		// ---------------------
		// MARK: -
		// MARK: Lines
		// ---------------------
		
		static Material *__Line3DMaterial;
		static Material *__Line2DMaterial;
		static Shader   *__LineShader;
		static Matrix    __LineTransform;
		
		static std::atomic<bool> __Line3DHandlerState;
		static std::atomic<bool> __Line2DHandlerState;
		
		static std::vector<Line3D> __Line3D;
		static std::vector<Line2D> __Line2D;
		
		static stl::thread_local_storage<Line3D> __Line3DStack;
		static stl::thread_local_storage<Line2D> __Line2DStack;
		
		template<typename Point, typename Vertex, typename Line>
		void DrawLine(Renderer *renderer, const std::vector<Line>& lines)
		{
			GLuint vbo, vao;
			
			gl::GenVertexArrays(1, &vao);
			gl::BindVertexArray(vao);
			
			gl::GenBuffers(1, &vbo);
			gl::BindBuffer(GL_ARRAY_BUFFER, vbo);
			
			// Enable the VAO state
			ShaderProgram *program = renderer->GetActiveProgram();
			
			gl::EnableVertexAttribArray(program->attPosition);
			gl::EnableVertexAttribArray(program->attColor0);
			
			for(const Line& line : lines)
			{
				gl::BufferData(GL_ARRAY_BUFFER, line.size() * sizeof(Point), line.data(), GL_STREAM_DRAW);
				
				gl::VertexAttribPointer(program->attPosition, 3, GL_FLOAT, false, sizeof(Point), (void *)0);
				gl::VertexAttribPointer(program->attColor0, 4, GL_FLOAT, false, sizeof(Point), (void *)sizeof(Vertex));
				
				gl::DrawArrays(GL_LINES, 0, static_cast<GLsizei>(line.size()));
				gl::BufferData(GL_ARRAY_BUFFER, line.size() * sizeof(Point), 0, GL_STREAM_DRAW);
			}
			
			gl::DisableVertexAttribArray(program->attPosition);
			gl::DisableVertexAttribArray(program->attColor0);
			
			// Clean up
			gl::BindBuffer(GL_ARRAY_BUFFER, 0);
			gl::BindVertexArray(0);
			
			gl::DeleteVertexArrays(1, &vao);
			gl::DeleteBuffers(1, &vbo);
			
			renderer->BindVAO(0);
		}
		
		void InstallLine3DHandler()
		{
			if(!__Line3DHandlerState.exchange(true))
			{
				RenderingObject object(RenderingObject::Type::Custom);
				object.material = __Line3DMaterial;
				object.callback = [](Renderer *renderer, const RenderingObject& object) {
					DrawLine<Point3D, Vector3, Line3D>(renderer, __Line3D);
					
					__Line3D.clear();
					__Line3DHandlerState.store(false);
				};
				
				Renderer::GetSharedInstance()->RenderDebugObject(object, Renderer::Mode::ModeWorld);
			}
		}
		
		void InstallLine2DHandler()
		{
			if(!__Line2DHandlerState.exchange(true))
			{
				RenderingObject object(RenderingObject::Type::Custom);
				object.material  = __Line2DMaterial;
				object.transform = &__LineTransform;
				
				object.callback = [](Renderer *renderer, const RenderingObject& object) {
					DrawLine<Point2D, Vector2, Line2D>(renderer, __Line2D);
					
					__Line2D.clear();
					__Line2DHandlerState.store(false);
				};
				
				object.prepare = [](Renderer *renderer, RenderingObject& object) {
					Camera *camera = renderer->GetActiveCamera();
					float height = camera->GetFrame().height - 1.0f;
					
					__LineTransform = Matrix::WithTranslation(Vector3(0.0f, height, 0.0f));
				};
				
				Renderer::GetSharedInstance()->RenderDebugObject(object, Renderer::Mode::ModeUI);
			}
		}
		
		void __AddLinePoint3D(const Vector3 &point, const Color& color)
		{
			Line3D &line = __Line3DStack.get();
			
			line.emplace_back(Point3D(point, color));
			
			if((line.size() % 2) == 0)
				line.emplace_back(Point3D(point, color));
		}
		
		void __AddLinePoint2D(const Vector2 &point, const Color& color)
		{
			Line2D &line = __Line2DStack.get();
			
			line.emplace_back(Point2D(point, color));
			
			if((line.size() % 2) == 0)
				line.emplace_back(Point2D(point, color));
		}
		
		
		void AddLinePoint(const Vector3& point, const Color& color)
		{
			__AddLinePoint3D(point, color);
		}
		
		void AddLinePoint(const Vector2& point, const Color& color)
		{
			__AddLinePoint2D(Vector2(point.x, -point.y), color);
		}
		
		void CloseLine()
		{
			Line3D &line3d = __Line3DStack.get();
			Line2D &line2d = __Line2DStack.get();
			
			if(!line3d.empty())
				line3d.erase(line3d.end() - 1);
			
			if(!line2d.empty())
				line2d.erase(line2d.end() - 1);
		}
		
		void EndLine()
		{
			Line3D &line3d = __Line3DStack.get();
			Line2D &line2d = __Line2DStack.get();
			
			if(!line3d.empty())
			{
				Line3D temp;
				
				line3d.erase(line3d.end() - 1);
				std::swap(temp, line3d);
				
				__Line3D.push_back(std::move(temp));
				InstallLine3DHandler();
			}
			
			if(!line2d.empty())
			{
				Line2D temp;
				
				line2d.erase(line2d.end() - 1);
				std::swap(temp, line2d);
				
				__Line2D.push_back(std::move(temp));
				InstallLine2DHandler();
			}
		}
		
		void DrawBox(const AABB& box, const Color& color)
		{
			Vector3 min = box.position + box.minExtend;
			Vector3 max = box.position + box.maxExtend;
			
			DrawBox(min, max, color);
		}
		
		void DrawBox(const Vector3& tmin, const Vector3& tmax, const Color& color)
		{
			Vector3 min = Vector3(std::min(tmin.x, tmax.x), std::min(tmin.y, tmax.y), std::min(tmin.z, tmax.z));
			Vector3 max = Vector3(std::max(tmin.x, tmax.x), std::max(tmin.y, tmax.y), std::max(tmin.z, tmax.z));
			
			// Top and bottom
			AddLinePoint(min, color);
			AddLinePoint(Vector3(max.x, min.y, min.z), color);
			AddLinePoint(Vector3(max.x, min.y, max.z), color);
			AddLinePoint(Vector3(min.x, min.y, max.z), color);
			AddLinePoint(min, color);
			CloseLine();
			
			AddLinePoint(Vector3(min.x, max.y, min.z), color);
			AddLinePoint(Vector3(max.x, max.y, min.z), color);
			AddLinePoint(Vector3(max.x, max.y, max.z), color);
			AddLinePoint(Vector3(min.x, max.y, max.z), color);
			AddLinePoint(Vector3(min.x, max.y, min.z), color);
			CloseLine();
			
			// Edges
			AddLinePoint(Vector3(min.x, min.y, min.z), color);
			AddLinePoint(Vector3(min.x, max.y, min.z), color);
			CloseLine();
			
			AddLinePoint(Vector3(max.x, min.y, min.z), color);
			AddLinePoint(Vector3(max.x, max.y, min.z), color);
			CloseLine();
			
			AddLinePoint(Vector3(min.x, min.y, max.z), color);
			AddLinePoint(Vector3(min.x, max.y, max.z), color);
			CloseLine();
			
			AddLinePoint(Vector3(max.x, min.y, max.z), color);
			AddLinePoint(Vector3(max.x, max.y, max.z), color);
			EndLine();
		}
		
		void DrawSphere(const Sphere& sphere, const Color& color, const int tesselation)
		{
			DrawSphere(sphere.position+sphere.offset, sphere.radius, Color::Yellow(), tesselation);
		}
		
		void DrawSphere(const Vector3 &pos, const float radius, const Color &color, const int tesselation)
		{
			Quaternion rot;
			Vector3 point(0.0f, 0.0f, radius);
			Vector3 temp;
			float offset = 360.0f/tesselation;
			float halfoffset = 180.0f/int(tesselation*0.5f);
			for(float pan = 0.0f; pan <= 180.0f; pan += halfoffset)
			{
				for(float tilt = 0.0f; tilt <= 360.0f; tilt += offset)
				{
					rot = Quaternion::WithEulerAngle(Vector3(pan, tilt, 0.0f));
					temp = pos+rot.GetRotatedVector(point);
					AddLinePoint(temp, color);
				}
				CloseLine();
			}
			
			for(float tilt = -90.0f+halfoffset; tilt < 90.0f; tilt += halfoffset)
			{
				for(float pan = 0.0f; pan <= 360.0f; pan += offset)
				{
					rot = Quaternion::WithEulerAngle(Vector3(pan, tilt, 0.0f));
					temp = pos+rot.GetRotatedVector(point);
					AddLinePoint(temp, color);
				}
				CloseLine();
			}
			
			EndLine();
		}
		
		// ---------------------
		// MARK: -
		// MARK: Helper
		// ---------------------
		
		void InstallDebugDraw()
		{
			__Line3DHandlerState.store(false);
			__Line2DHandlerState.store(false);
			
			__LineShader = new Shader();
			__LineShader->SetShaderForType("shader/rn_DebugDraw.vsh", ShaderType::VertexShader);
			__LineShader->SetShaderForType("shader/rn_DebugDraw.fsh", ShaderType::FragmentShader);
			
			__Line3DMaterial = new Material(__LineShader);
			__Line3DMaterial->Define("RN_DEBUG_3D");
			__Line3DMaterial->SetBlending(false);
			__Line3DMaterial->SetDepthWrite(false);
			__Line3DMaterial->SetDepthTest(true);
			__Line3DMaterial->SetLighting(false);
			
			__Line2DMaterial = new Material(__LineShader);
			__Line2DMaterial->Define("RN_DEBUG_2D");
			__Line2DMaterial->SetBlending(false);
			__Line2DMaterial->SetDepthWrite(false);
			__Line2DMaterial->SetDepthTest(false);
			__Line2DMaterial->SetLighting(false);
		}
	}
}
