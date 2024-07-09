#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

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
		ImGui::Text("Last render: %.3fms", mAveRenderTime);
		if (ImGui::Button("Render")) {
			Render();
		}
		ImGui::End();

		// Viewport (where we see the rendered image)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		mViewportWidth = ImGui::GetContentRegionAvail().x;
		mViewportHeight = ImGui::GetContentRegionAvail().y;
		
		if (mImage)
    	ImGui::Image(mImage->GetDescriptorSet(), {(float)mImage->GetWidth(), (float)mImage->GetHeight()});
		
		ImGui::End(); // end viewport
		ImGui::PopStyleVar();
		Render();
	}
	
	void Render() {
		Timer timer;

		if (!mImage || mViewportWidth != mImage->GetWidth() || mViewportHeight != mImage->GetHeight()) {
			mImage = std::make_shared<Image>(mViewportWidth, mViewportHeight, ImageFormat::RGBA);
			delete[] mImageData;
			mImageData = new uint32_t[mViewportWidth * mViewportHeight];
		}
		for (auto i = 0; i < mViewportWidth * mViewportHeight; i++) {
			mImageData[i] = Random::UInt();
			mImageData[i] |= 0xff000000;
		}
		mImage->SetData(mImageData);
		mLastRenderTime = timer.ElapsedMillis();
		mTotalRenderTime += mLastRenderTime;
		mRenderCount += 1;
	}

private:
	bool mShowMessageBox = false;
	std::shared_ptr<Image> mImage;
	uint32_t* mImageData = nullptr;
	uint32_t mViewportWidth = 0, mViewportHeight = 0;
	float mLastRenderTime = 0.0f;
	float mTotalRenderTime = 0.0f;
	int mRenderCount = 1;
	float mAveRenderTime = 0;
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