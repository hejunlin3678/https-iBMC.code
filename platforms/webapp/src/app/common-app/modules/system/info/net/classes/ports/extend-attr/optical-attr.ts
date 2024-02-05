
export class OpticalAttr {
    public manufacturer = null;
    public serialNumber = null;
    public productionDate = null;
    public partNumber = null;
    public packagingType = null;
    public mediumMode = null;
    public transceiverType = null;
    public supportedSpeedsMbps = null;
    public deviceType = null;
    public connectorType = null;
    public transferDistance = null;
    public rxLosState = null;
    public txFaultState = null;
    public waveLengthNanometer = null;
    public indentifier = null;
    constructor(
        manufacturer: string,
        serialNumber: string,
        productionDate: string,
        partNumber: string,
        packagingType: string,
        mediumMode: string,
        transceiverType: string,
        supportedSpeedsMbps: string,
        deviceType: string,
        connectorType: string,
        transferDistance: string,
        rxLosState: string,
        txFaultState: string,
        waveLengthNanometer: string,
        indentifier: string
    ) {
        this.manufacturer = ['COMMON_MANUFACTURER', manufacturer];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        this.productionDate = ['PRODUCTION_DATE', productionDate];
        this.partNumber = ['PART_NAME', partNumber];
        this.packagingType = ['PACKAGING_TYPE', packagingType];
        this.mediumMode = ['MEDIUM_MODE', mediumMode];
        this.transceiverType = transceiverType ? ['TRANSCEIVER_TYPE', transceiverType] : null;
        this.supportedSpeedsMbps = supportedSpeedsMbps ? ['NET_SPEED_RATE', supportedSpeedsMbps] : null;
        this.deviceType = ['COMMON_DEVICE_TYPE', deviceType];
        this.connectorType = ['CONNECTOR_TYPE', connectorType];
        this.transferDistance = ['TRANSFER_DISTANCE', transferDistance];
        this.rxLosState = ['RX_LOS_STATE', rxLosState];
        this.txFaultState = ['TX_FAULT_STATE', txFaultState];
        this.waveLengthNanometer = waveLengthNanometer ? ['WAVE_LENGTH', waveLengthNanometer] : null;
        this.indentifier = indentifier ? ['INDENTIFIER', indentifier] : null;
    }
}
