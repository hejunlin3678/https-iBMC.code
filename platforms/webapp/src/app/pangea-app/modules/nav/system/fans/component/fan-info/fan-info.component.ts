import { Component, Input } from '@angular/core';
import { FanInfo } from '../../model/fanInfo';
import { TranslateService } from '@ngx-translate/core';
import { IFanLabel } from '../../interface';

@Component({
    selector: 'app-fan-info',
    templateUrl: './fan-info.component.html',
    styleUrls: ['./fan-info.component.scss']
})
export class FanInfoComponent {

    constructor(
        private translate: TranslateService,
    ) {
        this.switchState = false;
    }

    public switchState: boolean;

    @Input() fanInfoList: FanInfo[];
    public fanLabel: IFanLabel = {
        model: this.translate.instant('FAN_HEAT_TYPE'),
        speed: this.translate.instant('FAN_HEAT_ROTATIONAL_SPEED'),
        rate: this.translate.instant('FAN_HEAT_RATE'),
        partNumber: this.translate.instant('OTHER_PART_NUMBER')
    };
}
