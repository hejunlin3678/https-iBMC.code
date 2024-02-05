import { Pipe, PipeTransform } from '@angular/core';
import { Health, StateImgs } from 'src/app/emm-app/models/common.datatype';

@Pipe({name: 'healthImagePipe'})
export class HealthImagePipe implements PipeTransform {
    transform(val) {
        switch (val) {
            case Health.Ok: {
                return StateImgs.okImgPath;
            }
            case Health.Warning: {
                return StateImgs.warnImgPath;
            }
            case Health.Critical: {
                return StateImgs.noImgPath;
            }
            default: return;
        }
    }
}
