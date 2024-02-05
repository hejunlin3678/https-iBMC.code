export class AEPInfo {

    private remainPercent: [string, string];
    private mediumTemp: [string, string];
    private revision: [string, string];
    private volatileMib: [string, string];
    private controlTemp: [string, string];
    private persistMib: [string, string];

    constructor(
        remainPercent: string,
        mediumTemp: string,
        revision: string,
        volatileMib: string,
        controlTemp: string,
        persistMib: string,
    ) {
        this.remainPercent = ['MEMORY_RESIDUAL_LIFE', remainPercent];
        this.mediumTemp = ['MEMORY_MEDIUM_TEMPERATURE', mediumTemp];
        this.revision = ['MEMORY_FIRMWARE_VERSION', revision];
        this.volatileMib = ['MEMORY_VOLATILE_CAPACITY', volatileMib];
        this.controlTemp = ['MEMORY_CONTROLLER_TEMPERATURE', controlTemp];
        this.persistMib = ['MEMORY_NON_VOLATILE_CAPACITY', persistMib];
    }
}
