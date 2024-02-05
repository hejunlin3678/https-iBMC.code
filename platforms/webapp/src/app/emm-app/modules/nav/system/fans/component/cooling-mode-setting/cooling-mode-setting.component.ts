import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { ISpeedAdjustmentMode } from '../../model/fans.interface';

@Component({
    selector: 'app-cooling-mode-setting',
    templateUrl: './cooling-mode-setting.component.html',
    styleUrls: ['./cooling-mode-setting.component.scss']
})
export class CoolingModeSettingComponent implements OnInit {

    constructor(
        private translate: TranslateService
    ) { }

    @Input() coolingModeSetting;
    @Input() param;

    public temperatureRangeList: { label: string }[];

    public saveBtn: boolean = true;
    public energySaving: ISpeedAdjustmentMode = {
        id: 'EnergySaving',
        label: this.translate.instant('FAN_MODEL_TITLE1'),
        content: this.translate.instant('FAN_MODEL_CONTENT1'),
        isSelect: false,
        isShow: true
    };
    public lowNoise: ISpeedAdjustmentMode = {
        id: 'LowNoise',
        label: this.translate.instant('FAN_MODEL_TITLE2'),
        content: this.translate.instant('FAN_MODEL_CONTENT2'),
        isSelect: false,
        isShow: true
    };
    public highPerformance: ISpeedAdjustmentMode = {
        id: 'HighPerformance',
        label: this.translate.instant('FAN_MODEL_TITLE3'),
        content: this.translate.instant('FAN_MODEL_CONTENT3'),
        isSelect: false,
        isShow: true
    };
    public selected(mode: ISpeedAdjustmentMode) {
        this.energySaving.isSelect = false;
        this.lowNoise.isSelect = false;
        this.highPerformance.isSelect = false;
        mode.isSelect = true;
        if (mode.id === this.coolingModeSetting) {
            this.param = null;
            this.saveBtn = true;
        } else {
            this.param = {
                FanSmartCoolingMode: mode.id
            };
            this.saveBtn = false;
        }
    }
    private getFanMode() {
        switch (this.coolingModeSetting) {
            case 'EnergySaving':
                // 节能模式
                this.energySaving.isSelect = true;
                break;
            case 'LowNoise':
                // 低噪音模式
                this.lowNoise.isSelect = true;
                break;
            case 'HighPerformance':
                // 高性能模式
                this.highPerformance.isSelect = true;
                break;
            default:
                break;
        }
    }
    ngOnInit(): void {
        this.getFanMode();
    }
    /**
     * 模板中实际调用的是Modal服务提供的close和dismiss方法，并非此处定义的方法；
     * 在此处定义close和dismiss方法只是为了避免生产环境打包时报错
     */
    close(): void {
    }
    dismiss(): void {
    }

}
