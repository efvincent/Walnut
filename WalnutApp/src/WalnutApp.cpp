#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"
#include "Renderer.h"

#define UPDATE_RENDER_TIME_AT 25

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		// Render();
		ImGui::Begin("Settings");
		if (mRenderCount >= UPDATE_RENDER_TIME_AT) {
			mAveRenderTime = mTotalRenderTime / mRenderCount;
			mTotalRenderTime = 0;
			mRenderCount = 0;
		}
		auto lblPlayButton = mPaused ? "Play" : "Pause";
		ImGui::Text("Last render: %.3fms", mAveRenderTime);
		if (ImGui::Button(lblPlayButton)) {
			mPaused = !mPaused;
		}

    // Color picker
    if (ImGui::ColorPicker4("Color", &mColor.x)) {
        // This block is executed when the color changes.
        // You can add code here to handle the color change if necessary.
    }

		ImGui::End();

		// Viewport (where we see the rendered image)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		if (!mPaused) 
		{
			mViewportWidth = ImGui::GetContentRegionAvail().x;
			mViewportHeight = ImGui::GetContentRegionAvail().y;
			
			Render();
		}

		auto image = mRenderer.getFinalImage();
		if (image)
    	ImGui::Image(
				image->GetDescriptorSet(), 
				{ (float)image->GetWidth(), 
					(float)image->GetHeight() } ,
				ImVec2(0, 1),	 
				ImVec2(1, 0) 	// flips the image on the y axis
			);		
	
		ImGui::End(); // end viewport
		ImGui::PopStyleVar();
	}
	
	void Render() {
		Timer timer;
		mRenderer.OnResize(mViewportWidth, mViewportHeight);
		glm::vec3 lightDir = glm::vec3(-1);
		glm::vec3 sphereOrigin = glm::vec3(0);
		mRenderer.Render(mColor, lightDir, sphereOrigin);

		mLastRenderTime = timer.ElapsedMillis();
		mTotalRenderTime += mLastRenderTime;
		mRenderCount += 1;
	}

private:
	Renderer mRenderer;
	bool      mShowMessageBox  = false;
	uint32_t  mViewportWidth   = 0,
	          mViewportHeight  = 0;
	float     mLastRenderTime  = 0.0f;
	float     mTotalRenderTime = 0.0f;
	int       mRenderCount     = 1;
	float     mAveRenderTime   = 0;
	bool      mPaused          = false;
	glm::vec4 mColor           = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut Example";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}