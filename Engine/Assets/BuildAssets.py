import shutil
import subprocess
import sys
from pathlib import Path

# This script builds shaders and copies all assets included with the engine
# into the binary output directory

shaders = [
    "DefaultShader.frag",
    "DefaultShader.vert"
]

def build_shaders():
    shader_assets_dir = f"{assets_dir}/Shaders"
    shaders_output_dir = Path(f"{output_path}/Shaders")
    if not shaders_output_dir.exists():
        shaders_output_dir.mkdir(parents=True, exist_ok=True)

    for shader in shaders:
        subprocess.run(["glslc", f"{shader_assets_dir}/{shader}", "-o", f"{shaders_output_dir}/{shader}.spv"])

def copy_assets_to_output():
    shutil.copytree(f"{assets_dir}/Models", f"{output_path}/Models", dirs_exist_ok=True)
    shutil.copytree(f"{assets_dir}/Textures", f"{output_path}/Textures", dirs_exist_ok=True)
    shutil.copytree(f"{assets_dir}/Scenes", f"{output_path}/Scenes", dirs_exist_ok=True)

if __name__ == "__main__":
    if len(sys.argv) < 4: exit("Failed to receive required cmake variables!")

    assets_dir = Path(sys.argv[1])
    build_type = Path(sys.argv[2])
    output_path = Path(f"{sys.argv[3]}/EngineAssets")

    if not output_path.exists():
        output_path.mkdir(parents=True, exist_ok=True)

    build_shaders()
    copy_assets_to_output()
