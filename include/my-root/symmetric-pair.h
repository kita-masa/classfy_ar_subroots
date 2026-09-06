
#include "root-system.h"

decorated_root_system symmetric_pair_from_grading(const decorated_root_system &system, const root& highest_root);
decorated_root_system associated_pair(const decorated_root_system& system, const std::vector<root>& symmetric_sub);
bool is_symmetric_pair(const decorated_root_system& system, const decorated_root_system& subsystem);