#ifndef UCTRegion_hh
#define UCTRegion_hh

#include <vector>
#include <iostream>
#include <memory>

#include "UCTTower.hh"

namespace l1tcalo {
  constexpr uint32_t RegionETMask{0x000003FF};
  constexpr uint32_t RegionEGVeto{0x00000400};
  constexpr uint32_t RegionTauVeto{0x00000800};
  constexpr uint32_t HitTowerBits{0x0000F000};
  constexpr uint32_t RegionNoBits{0x000F0000};
  constexpr uint32_t CardNoBits{0x00700000};
  constexpr uint32_t CrateNoBits{0x01800000};
  constexpr uint32_t NegEtaBit{0x80000000};
  constexpr uint32_t LocationBits{0xFFFFF000};
  constexpr uint32_t LocationShift{12};
  constexpr uint32_t RegionNoShift{16};
  constexpr uint32_t CardNoShift{20};
  constexpr uint32_t CrateNoShift{23};
}  // namespace l1tcalo

class UCTRegion {
public:
  UCTRegion(uint32_t crt, uint32_t crd, bool ne, uint32_t rgn, int fwv);

  // No default constructor is needed

  UCTRegion() = delete;

  //copy constructor helps gettting around some memory leakage
  // and ownership problems in the summary card emulation
  UCTRegion(const UCTRegion&);

  // No equality operator is needed

  const UCTRegion& operator=(const UCTRegion&) = delete;

  virtual ~UCTRegion() = default;

  // To setData for towers before processing

  const std::vector<std::shared_ptr<UCTTower>>& getTowers() { return towers; }

  // To process event

  bool clearEvent();
  bool setECALData(UCTTowerIndex t, bool ecalFG, uint32_t ecalET);
  bool setHCALData(UCTTowerIndex t, uint32_t hcalFB, uint32_t hcalET);
  bool setRegionSummary(
      uint16_t regionData);  // Use when the region collection is available and no direct access to TPGs
  bool process();

  // Packed data access

  const uint32_t rawData() const { return regionSummary; }
  const uint32_t location() const { return ((regionSummary & l1tcalo::LocationBits) >> l1tcalo::LocationShift); }

  const int hitCaloEta() const {
    uint32_t hitTowerLocation = (location() & 0xF);
    return towers[hitTowerLocation]->caloEta();
  }

  const int hitCaloPhi() const {
    uint32_t hitTowerLocation = (location() & 0xF);
    return towers[hitTowerLocation]->caloPhi();
  }

  const UCTTowerIndex hitTowerIndex() const { return UCTTowerIndex(hitCaloEta(), hitCaloPhi()); }

  const UCTRegionIndex regionIndex() const {
    UCTGeometry g;
    return UCTRegionIndex(g.getUCTRegionEtaIndex(negativeEta, region), g.getUCTRegionPhiIndex(crate, card));
  }

  const uint32_t compressedData() const { return regionSummary; }

  // Access functions for convenience
  // Note that the bit fields are limited in hardware

  const uint32_t et() const { return (l1tcalo::RegionETMask & regionSummary); }
  const bool isEGammaLike() const { return !((l1tcalo::RegionEGVeto & regionSummary) == l1tcalo::RegionEGVeto); }
  const bool isTauLike() const { return !((l1tcalo::RegionTauVeto & regionSummary) == l1tcalo::RegionTauVeto); }

  // More access functions

  const uint32_t getCrate() const { return crate; }
  const uint32_t getCard() const { return card; }
  const uint32_t getRegion() const { return region; }

  const bool isNegativeEta() const { return negativeEta; }

  const std::shared_ptr<UCTTower> getTower(UCTTowerIndex t) const { return getTower(t.first, t.second); }

  friend std::ostream& operator<<(std::ostream&, const UCTRegion&);

protected:
  // Helper functions

  const std::shared_ptr<UCTTower> getTower(uint32_t caloEta, uint32_t caloPhi) const;

  // Region location definition

  uint32_t crate;
  uint32_t card;
  uint32_t region;
  bool negativeEta;

  // Owned region level data

  std::vector<std::shared_ptr<UCTTower>> towers;

  uint32_t regionSummary;

  const int fwVersion;
};

#endif
