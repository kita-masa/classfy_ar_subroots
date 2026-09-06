
#include <iostream>
#include <vector>
#include <string>

#include "root-system.h"

void serialize_embedding(const std::vector<root>& embedding, const decorated_root_system& system, const std::string diagram_identifier, std::ostream& out);

std::tuple<std::string, decorated_root_system, std::vector<root>> deserialize_embedding(std::istream& in);

std::vector<root> find_standard_simple_roots_su(const decorated_root_system& system);

struct embedding_root_data {
	decorated_root_system new_system;
	std::vector<root> additional_roots;
};

class embedding_root_helper {
protected:
	decorated_root_system system;
	std::vector<root> embedded_simple_roots;
	root lowest_root;

	std::vector<root> candidates;
	bool can_add(const root& r, const std::vector<root>& test_roots) const;
	void dfs(std::vector<root>& additional_roots, int pos, std::vector<embedding_root_data>& result);
public:
	embedding_root_helper(const decorated_root_system& system, const std::vector<root>& embedded_simple_roots);
	void fix_lowest_root(const root& r);
	std::vector<embedding_root_data> search();
};

std::vector<decorated_root_system> standard_positive_system_for_embedding(const std::vector<root>& embedding, const decorated_root_system& system, bool remove_noncompact = false);