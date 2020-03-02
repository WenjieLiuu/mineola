#include "../include/AppFrame.h"
#include <memory>
#include <iostream>
#include "../../include/AppHelper.h"
#include "../../include/CameraController.h"
#include "../../include/SceneLoader.h"
#include "../../include/MeshIO.h"
#include "../../include/FileSystem.h"
#define MINEOLA_USE_STBIMAGE
#include "../../include/STBImagePlugin.h"
#include "../../include/GLTFLoader.h"
#include "../../include/AnimatedEntity.h"

namespace {

static const std::string kSceneFilename = "gltfscene.json";
static const std::string kConfigPrefix = R"({
  "geometries": [
    {
      "filename": ")";

static const std::string kConfigSuffix = R"(",
      "effect": "mineola:effect:pbr_srgb",
      "shadowmap": false,
      "node": "geometry",
      "layer": 0
    }
  ]
})";

}

namespace mineola_pc {
using namespace mineola;

void OnKey(uint32_t key, uint32_t action) {
  if (action == Engine::BUTTON_UP) {
    auto &en = Engine::Instance();
    if (key == 'Q') {
      en.SignalTermination();
    } else if (key == 'P') {
      en.EntityMgr().Transform([](const std::string &name, auto &entity) {
        auto anim_entity = bd_cast<AnimatedEntity>(entity);
        if (anim_entity) {
          anim_entity->Play();
        }
      });

    } else if (key == 'S') {
      static double offset = 0.0;
      en.EntityMgr().Transform([](const std::string &name, auto &entity) {
        auto anim_entity = bd_cast<AnimatedEntity>(entity);
        if (anim_entity) {
          anim_entity->Snapshot(offset);
        }
      });
      offset += 10.0;

    } else if (key == 'R') {
    }
  }
}

class GLTFViewerApp : public mineola::AppFrame,
  public std::enable_shared_from_this<GLTFViewerApp> {
public:
  GLTFViewerApp() {}
  virtual ~GLTFViewerApp() {}

  virtual bool InitScene() override {
    AppFrame::InitScene();

    Engine &en = GetEngine();

    if (!gltf_filename_.empty()) {
      namespace ph = std::placeholders;
      auto gltf_loader = std::bind(gltf::LoadScene,
        STBLoadImageFromFile, STBLoadImageFromMem,
        ph::_1, ph::_2, ph::_3, ph::_4, ph::_5);

      BuildSceneFromConfigFile(kSceneFilename.c_str(), {gltf_loader});
      auto cfg = kConfigPrefix + gltf_filename_ + kConfigSuffix;

      BuildSceneFromConfig(cfg.c_str(), {gltf_loader});
    }

    cam_ctrl_.reset(new ArcballController);
    cam_ctrl_->BindToNode("camera");
    cam_ctrl_->Activate();
    bd_cast<ArcballController>(cam_ctrl_)->SetSpeed(0.1f);

    en.AddKeyboardCallback(OnKey);

    en.ChangeCamera("main", false);

    return true;
  }

  void SetFilename(const char *filename) {
    gltf_filename_ = filename;
  }

private:
  std::string gltf_filename_;
  std::shared_ptr<mineola::CameraController> cam_ctrl_;
};

}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: gltfdemo (gltf filename)" << std::endl;
    return 0;
  }

  std::shared_ptr<mineola_pc::GLTFViewerApp> app(new mineola_pc::GLTFViewerApp());
  app->SetFilename(argv[1]);
  app->Run(512, 512);
  return 0;
}
