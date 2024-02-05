import { Pipe, PipeTransform } from '@angular/core';
import { PowerState, StateImgs } from 'src/app/emm-app/models/common.datatype';

@Pipe({name: 'powerStateImagePipe'})
export class PowerStateImagePipe implements PipeTransform {
    transform(val) {
        switch (val) {
            case PowerState.On: {
                return StateImgs.onImgPath;
            }
            case PowerState.Off: {
                return StateImgs.offImgPath;
            }
            case PowerState.VersionLow: {
                return StateImgs.verlowImgPath;
            }
            case PowerState.NotConnected: {
                return StateImgs.unKnownImgPath;
            }
            default: return;
        }
    }
}
