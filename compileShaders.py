from pathlib import Path

SHADER_FOLDER_PATH = Path("source/shaders")

def main():
    output_string="#pragma once\n"
    #compile the shaders into one string
    for glsl_file in SHADER_FOLDER_PATH.glob("*.glsl"):
        print(f"Found shader: {glsl_file.name}")
        with open(glsl_file, "r") as f:
            shader_code = f.read().replace("\n","\\n")
            shader_name=glsl_file.name.replace(".","_").upper()
            output_string+=f"#define {shader_name} \"{shader_code}\"\n"

    # Write to shaders.h in the same folder
    output_file = SHADER_FOLDER_PATH / "shaders.h"
    with open(output_file, "w", encoding="utf-8") as out_file:
        out_file.write(output_string)

    print(f"Wrote shader header to {output_file}")
main()