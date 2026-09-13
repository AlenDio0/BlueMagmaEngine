set(BM_SHADER_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/Shader")
set(BM_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated/Shader")
file(MAKE_DIRECTORY  "${BM_GENERATED_DIR}")

function(bm_generate_shader VAR_NAME SHADER_FILE)
    set(INPUT  "${BM_SHADER_DIR}/${SHADER_FILE}")
    set(OUTPUT "${BM_GENERATED_DIR}/${VAR_NAME}.hpp")

    add_custom_command(
        OUTPUT  "${OUTPUT}"
        COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE=${INPUT}
            -DOUTPUT_FILE=${OUTPUT}
            -DVAR_NAME=${VAR_NAME}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/GenerateShader.cmake"
        DEPENDS "${INPUT}"
        COMMENT "Generating Shader '${SHADER_FILE}'"
        VERBATIM
    )
    set_source_files_properties("${OUTPUT}" PROPERTIES GENERATED TRUE)
    target_sources(BlueMagma PRIVATE "${OUTPUT}")
endfunction()

# Shaders

bm_generate_shader(RectFrag "Rect.frag")
bm_generate_shader(CircleFrag "Circle.frag")

target_include_directories(BlueMagma PRIVATE "${BM_GENERATED_DIR}/..")
