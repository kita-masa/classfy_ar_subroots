#pragma once

#include "diagram.h"

dynkin_diagram dynkin_diagram_A(size_t rank);
dynkin_diagram dynkin_diagram_G2();
dynkin_diagram dynkin_diagram_F4();
dynkin_diagram dynkin_diagram_E6();
dynkin_diagram dynkin_diagram_E7();
dynkin_diagram dynkin_diagram_E8();

vogan_diagram vogan_diagram_sl_real(int n);
vogan_diagram vogan_diagram_su(int p, int q);

vogan_diagram vogan_diagram_so(int p, int q);

vogan_diagram vogan_diagram_G2_split();

vogan_diagram vogan_diagram_F4_split();
vogan_diagram vogan_diagram_F4_rank1();

vogan_diagram vogan_diagram_E6_split();
vogan_diagram vogan_diagram_E6_quaternionic();
vogan_diagram vogan_diagram_E6_hermitian();
vogan_diagram vogan_diagram_E6_other();

vogan_diagram vogan_diagram_E7_split();
vogan_diagram vogan_diagram_E7_quaternionic();
vogan_diagram vogan_diagram_E7_hermitian();

vogan_diagram vogan_diagram_E8_split();
vogan_diagram vogan_diagram_E8_quaternionic();


std::vector<std::string> exceptional_vogan_diagram_identifiers();
bool test_vogan_diagram_identifier(const std::string& s);
vogan_diagram vogan_diagram_from_string(const std::string& s);