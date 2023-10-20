#include <vector>
#include <string>

const size_t k_max_msg = 4096;

int32_t query(int fd, const char *text);

// EVENT LOOP
// int32_t send_req(int fd, const char *text);

int32_t send_req(int fd, const std::vector<std::string> &cmd);
int32_t read_res(int fd);

int32_t on_response(const uint8_t *data, size_t size);
