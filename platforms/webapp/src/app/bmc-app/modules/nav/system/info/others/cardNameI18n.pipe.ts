import { Pipe, PipeTransform } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { GlobalDataService, PRODUCTTYPE } from 'src/app/common-app/service';
import { CardUrlArray } from './models/cardUrlArray';

@Pipe({
    name: 'cardNameI18n'
})
export class CardNameI18nPipe implements PipeTransform {
    constructor(
        private i18n: TranslateService,
        private globalData: GlobalDataService
    ) { }
    transform(cardUrlArray: CardUrlArray, args?: any): string {
        let name = '';
        switch (cardUrlArray.getName) {
            case 'DiskBackplane':
                name = this.i18n.instant('OTHER_DISK_BACK_PLANE');
                break;
            case 'PCIeRiserCard':
                name = this.i18n.instant('OTHER_RISER_CARD');
                break;
            case 'CICCard':
                name = this.i18n.instant('OTHER_CIC_CARD');
                break;
            case 'RAIDCard':
                name = this.i18n.instant('OTHER_RAID_CARD');
                break;
            case 'M2TransformCard':
                name = this.i18n.instant('OTHER_M2_TRANSFORM_CARD');
                break;
            case 'PeripheralCard':
                name = this.i18n.instant('OTHER_PERIPHERAL_CARD');
                break;
            case 'PassThroughCard':
                name = this.i18n.instant('OTHER_PASS_THROUGH_CARD');
                break;
            case 'MezzCard':
                name = this.i18n.instant('OTHER_MEZZCARD');
                break;
            case 'PCIeTransformCard':
                name = this.i18n.instant('OTHER_PCIE_TRANSFORM_CARD');
                break;
            case 'GPUBoard':
                name = this.i18n.instant('OTHER_GPU_BOARD');
                break;
            case 'FanBackplane':
                name = this.i18n.instant('OTHER_FAN_BACKPLANE');
                if (this.globalData.productType === PRODUCTTYPE.TIANCHI || this.globalData.productType === PRODUCTTYPE.WUHAN) {
                    name += ' (1/1)';
                }
                break;
            case 'PowerBoard':
                name = this.i18n.instant('OTHER_POWER_BOARD');
                break;
            case 'HorizontalConnectionBoard':
                name = this.i18n.instant('OTHER_HORIZONTAL_CONNECTION_BOARD');
                break;
            case 'CPUBoard':
                name = this.i18n.instant('OTHER_CPU_BOARD');
                break;
            case 'MemoryBoard':
                name = this.i18n.instant('OTHER_MEMORY_BOARD');
                break;
            case 'IOBoard':
                name = this.i18n.instant('OTHER_IO_BOARD');
                break;
            case 'ChassisBackplane':
                name = this.i18n.instant('OTHER_MACHINE_FRAME_BACKPLANE');
                break;
            case 'mainboardSDCard':
                name = this.i18n.instant('OTHER_SD_CARD');
                break;
            case 'PCIeCard':
                name = this.i18n.instant('OTHER_PCIE_CARD');
                break;
            case 'OCPCard':
                name = this.i18n.instant('OTHER_OCP_CARD');
                break;
            case 'BBUModule':
                name = this.i18n.instant('OTHERS_BBU');
                break;
            case 'ExpandBoard':
                name = this.i18n.instant('OTHERS_EXPANDER');
                if (this.globalData.productType === PRODUCTTYPE.TIANCHI) {
                    name = this.i18n.instant('OTHERS_EXPANDSION_BOARD');
                }
                break;
            case 'SecurityModule':
                name = this.i18n.instant('OTHER_SECURITY_MODULE');
                break;
            case 'PeripheralFirmware':
                name = this.i18n.instant('OTHER_PERIPHERAL_FIRMWARE');
                break;
            case 'LCD':
                name = this.i18n.instant('OTHER_LCD');
                break;
            case 'LeakageDetectionCard':
                name = this.i18n.instant('OTHER_LEAKAGECARD_CONTROLLER');
                break;
            case 'SDStorage':
                name = this.i18n.instant('OTHER_SD_CONTROLLER');
                break;
            default:
                break;
        }
        if (cardUrlArray.getMaxCount > 0) {
            name = name + ' (' + cardUrlArray.getTotalCount + '/' + cardUrlArray.getMaxCount + ')';
        }
        return name;
    }
}
