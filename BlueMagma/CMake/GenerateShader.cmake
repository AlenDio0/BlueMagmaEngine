file(READ "${INPUT_FILE}" SHADER_CONTENT)
file(WRITE "${OUTPUT_FILE}"
"//
// Auto-generated from ${INPUT_FILE}
//

#pragma once

namespace BM::Shader
{
    constexpr inline const char* ${VAR_NAME} = R\"(
${SHADER_CONTENT}
)\";
}")
