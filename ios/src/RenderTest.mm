//
//  TestRenderApp.m
//  mineola
//
//  Created by Fangyang Shen on 2018/9/11.
//  Copyright © 2018 Fangyang Shen. All rights reserved.
//
#import "RenderTest.h"
#include <string>
#include <mineola/glutility.h>
#include <mineola/Engine.h>
#include <mineola/CameraController.h>
#include <mineola/SceneLoader.h>
#include <mineola/GLTFLoader.h>
#include <mineola/AnimatedEntity.h>
#define MINEOLA_USE_STBIMAGE
#include <mineola/STBImagePlugin.h>

namespace {

static const std::string kSceneFilename = "gltf.json";
static const std::string kConfigStr = R"({
  "geometries": [
    {
      "filename": "CesiumMan.glb",
      "effect": "mineola:effect:pbr_srgb",
      "shadowmap": false,
      "node": "geometry",
      "layer": 0
    }
  ]
})";

}

@interface RenderTest() {
  std::shared_ptr<mineola::CameraController> _camCtrl;
}

@end

@implementation RenderTest

- (void)initScene {
  using namespace mineola;

  auto &en = Engine::Instance();

  std::string resource_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
  en.ResrcMgr().AddSearchPath(resource_path.c_str());

  en.SetExtTextureLoaders(STBLoadImageFromFile, STBLoadImageFromMem);

  BuildSceneFromConfigFile(kSceneFilename.c_str(), {gltf::LoadScene});
  BuildSceneFromConfig(kConfigStr.c_str(), {gltf::LoadScene});

  _camCtrl = std::make_shared<ArcballController>();
  _camCtrl->BindToNode("camera");
  _camCtrl->Activate();
  bd_cast<ArcballController>(_camCtrl)->SetSpeed(0.1f);

  en.ChangeCamera("main", false);

  en.EntityMgr().Transform([](const std::string &name, auto &entity) {
    auto anim_entity = bd_cast<AnimatedEntity>(entity);
    if (anim_entity) {
      anim_entity->SetPlayMode(AnimatedEntity::kPlayLoop);
      anim_entity->Play();
    }
  });
}

@end
