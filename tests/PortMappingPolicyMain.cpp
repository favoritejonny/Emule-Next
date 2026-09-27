// Standalone entry point for sanitizer checks; no MFC or networking required.
#include "PortMappingPolicyCases.h"
int main() { return SelfTestPortMapping() ? 0 : 1; }
