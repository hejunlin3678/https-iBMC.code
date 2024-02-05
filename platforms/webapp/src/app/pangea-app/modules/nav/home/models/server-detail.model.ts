import { IServerDetail } from '../home.datatype';
import { DeviceInfo } from './device-info.model';
import { IDeviceInfo } from './overview.datatype';

export class ServerDetail {
    private serverDetail: IServerDetail;

    constructor(deviceInfo: IDeviceInfo) {
        this.serverDetail = {
            productName: deviceInfo.productName,
            aliasName: deviceInfo.aliasName ? ` (${deviceInfo.aliasName})` : null,
            productImg: `/extern/custom/${deviceInfo.productPicture}.png`,
            errorImg: () => {
                this.serverDetail.productImg = `theme/custom/bmcimg/product/${deviceInfo.productPicture}.png`;
            },
            deviceInfo: new DeviceInfo(deviceInfo)
        };
    }
    get getServerDetail(): IServerDetail {
        return this.serverDetail;
    }
}
