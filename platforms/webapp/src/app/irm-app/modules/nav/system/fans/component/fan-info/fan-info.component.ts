import { Component, Input, OnDestroy } from '@angular/core';
import { FanInfo } from 'src/app/common-app/models/fanInfo';
import { TranslateService } from '@ngx-translate/core';
import { IFanLabel } from '../../interface';
import { FanInfoService } from './fan-info.service';

@Component({
    selector: 'app-fan-info',
    templateUrl: './fan-info.component.html',
    styleUrls: ['./fan-info.component.scss']
})
export class FanInfoComponent implements OnDestroy {

    constructor(
        private translate: TranslateService,
        private fanInfoService: FanInfoService
    ) {
        this.switchState = false;
    }

    public switchState: boolean;
    private timer: any;
    private timerCount: number = 0;

    @Input() fanInfoList: FanInfo[];
    public fanLabel: IFanLabel = {
        model: this.translate.instant('FAN_HEAT_TYPE'),
        speed: this.translate.instant('FAN_HEAT_ROTATIONAL_SPEED'),
        rate: this.translate.instant('FAN_HEAT_RATE'),
        partNumber: this.translate.instant('OTHER_PART_NUMBER')
    };
    ngOnDestroy(): void {
        this.clearTimer();
    }

    public switchChange(): void {
        if (this.switchState) {
            this.interval();
        } else {
            this.clearTimer();
        }
    }

    private clearTimer(): void {
        if (this.timer) {
            clearInterval(this.timer);
        }
        this.timerCount = 0;
    }

    private interval(): void {
        this.timer = setInterval(() => {
            if (this.timerCount < 7) {
                this.timerCount++;
                this.fanInfoService.getThermals().subscribe(
                    (fanInfo: FanInfo[]) => {
                        this.fanInfoList.forEach(
                            (fan) => {
                                const fanObj: FanInfo[] = fanInfo.filter((item) => item.id === fan.id);
                                fan.fanSpeed = fanObj[0] ? fanObj[0].fanSpeed : fan.fanSpeed;
                                fan.fanRate = fanObj[0] ? fanObj[0].fanRate : fan.fanRate;
                            }
                        );
                        this.timerCount--;
                    }
                );
            }
        }, 1000);
    }
}
