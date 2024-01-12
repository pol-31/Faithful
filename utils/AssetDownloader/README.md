TODO: precaution "file ${filename} will be rewritten"

struct AssetsInfo {
std::string zip_name;
std::string url;
std::vector<uint8_t> hash; // sha256
int redirection_count;
int audio_count;
int models_count;
int textures_count;
};

TODO: convention for assets archive:

/// Downloaded dir already has needed hierarchy:
/// in_dir/:
///       |----- audio/
///       |----- models/
///       |----- textures/