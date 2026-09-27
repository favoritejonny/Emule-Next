// eMule Next - GPL-2.0-or-later
#pragma once

// Read-only local checks. Zero means automatic router setup is eligible;
// otherwise the result is a localized resource ID explaining why it is not.
UINT GetAutomaticSetupRestriction();
// Read-only network checks, without reading mutable client preferences.
UINT GetAutomaticNetworkRestriction();
