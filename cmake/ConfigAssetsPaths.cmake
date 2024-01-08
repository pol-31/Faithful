macro(config_asset_paths)
    set(FAITHFUL_CONFIG_PATH "${CMAKE_SOURCE_DIR}/config")

    set(FAITHFUL_ASSET_PATH "${CMAKE_SOURCE_DIR}/assets")
    set(FAITHFUL_ASSET_AUDIO_PATH "${CMAKE_SOURCE_DIR}/assets/audio")
    set(FAITHFUL_ASSET_MODEL_PATH "${CMAKE_SOURCE_DIR}/assets/models")
    set(FAITHFUL_ASSET_SHADER_PATH "${CMAKE_SOURCE_DIR}/assets/shaders")
    set(FAITHFUL_ASSET_TEXTURE_PATH "${CMAKE_SOURCE_DIR}/assets/textures")
    set(FAITHFUL_ASSET_TEMP_TEXTURES_PATH "${CMAKE_SOURCE_DIR}/utils/AssetProcessor/temp")

    set(FAITHFUL_ASSET_INFO_FILE "${FAITHFUL_ASSET_PATH}/assets_info.txt")
    set(FAITHFUL_ASSET_AUDIO_INFO_FILE "${FAITHFUL_ASSET_AUDIO_PATH}/audio_info.txt")
    set(FAITHFUL_ASSET_MODEL_INFO_FILE "${FAITHFUL_ASSET_MODEL_PATH}/models_info.txt")
    set(FAITHFUL_ASSET_SHADER_INFO_FILE "${FAITHFUL_ASSET_SHADER_PATH}/shaders_info.txt")
    set(FAITHFUL_ASSET_TEXTURE_INFO_FILE "${FAITHFUL_ASSET_TEXTURE_PATH}/textures_info.txt")

    set(FAITHFUL_ASSET_URL "https://drive.google.com/uc?export=download&id=1Uuu2nKZG17t-u94f1TpcUo8IWxyHvpFo")

endmacro(config_asset_paths)