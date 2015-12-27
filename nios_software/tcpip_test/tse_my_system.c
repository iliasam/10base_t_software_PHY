#ifdef ALT_INICHE
    #include "ipport.h"
#endif

#define SOFT_PHY_OUI 1
#define SOFT_PHY_MODEL 1
#define SOFT_PHY_REV 1


#include "ipport.h"                   // MAXNETS
#include "system.h"                // component names
#include "altera_avalon_tse.h"  // phy_cfg_fp, alt_tse_system_info, TSE_PHY_AUTO_ADDRESS
#include "altera_avalon_tse_system_info.h"  // TSE_SYSTEM_EXT_MEM_NO_SHARED_FIFO

//DON'T FORGET TO SET in "altera_avalon_tse.h"
//ALTERA_TSE_DUPLEX_MODE_DEFAULT TSE_PHY_DUPLEX_HALF
//ALTERA_TSE_MAC_SPEED_DEFAULT TSE_PHY_SPEED_10

alt_u32 SOFT_PHY_link_status_read(np_tse_mac *pmac)
{
	alt_u32 link_status = 0;

	/* If speed == 10 Mbps */
	link_status |= 0x8;

	/* If duplex == Full */
	// link_status |= 0x1;

	return link_status;
}

void setup_phy(void)
{
	alt_tse_system_mac sys_mac = {TSE_SYSTEM_MAC(ETHERNET_1)};
	alt_tse_system_sgdma sys_sgdma = {TSE_SYSTEM_SGDMA(SGDMA_TX, SGDMA_RX)};
	alt_tse_system_desc_mem sys_mem = {TSE_SYSTEM_NO_DESC_MEM()};
	alt_tse_system_shared_fifo sys_sfifo = {TSE_SYSTEM_NO_SHARED_FIFO()};
	alt_tse_system_phy sys_phy = {TSE_SYSTEM_PHY(1, NULL)};//no mdio

	alt_tse_system_add_sys(&sys_mac, &sys_sgdma, &sys_mem, &sys_sfifo, &sys_phy);

	alt_tse_phy_profile SOFT_PHY = {"SOFT PHY",
			SOFT_PHY_OUI, /* OUI */
			SOFT_PHY_MODEL, /* Vender Model Number */
			SOFT_PHY_REV, /* Model Revision Number */
			0, /* Location of Status Register (ignored) */
			0, /* Location of Speed Status (ignored) */
			0, /* Location of Duplex Status (ignored) */
			0, /* Location of Link Status (ignored) */
			0, /* No function pointer */
			&SOFT_PHY_link_status_read /* Function pointer to read from PHY specific status register */
	};

	alt_tse_phy_add_profile(&SOFT_PHY);
}
