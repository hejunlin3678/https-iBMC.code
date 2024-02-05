import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { FanService } from './services/fan.service';
import { LoadingService, SystemLockService, UserInfoService, AlertMessageService} from 'src/app/common-app/service';
import { AirInletTemperature } from './model/airInletTemperature';
import { FanInfo } from './model/fanInfo';
import { TiModalService, TiModalRef } from '@cloud/tiny3';
import { IntelSpeedAdjustComponent } from './component/intel-speed-adjust/intel-speed-adjust.component';
import { IntelSpeedAdjust } from './model/intelSpeedAdjust';
import { getFanInfo } from './fan-ibmc.util';
import { TranslateService } from '@ngx-translate/core';

@Component({
  selector: 'app-fans',
  templateUrl: './fans.component.html',
  styleUrls: ['./fans.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class FansComponent implements OnInit {

    constructor(
        private service: FanService,
        private loading: LoadingService,
        private tiModal: TiModalService,
        private user: UserInfoService,
        private alert: AlertMessageService,
        private lockService: SystemLockService,
        private translate: TranslateService
    ) { }

    public airInletTemperature: AirInletTemperature = null;
    public fanInfoList: FanInfo[] = [];
    public intelSpeedAdjust: IntelSpeedAdjust = new IntelSpeedAdjust();
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock: boolean = false;
    public fanState: boolean = false;
    public fanModal;

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public openAdjust() {
        if (this.isSystemLock) {
            return;
        }
        this.fanModal = this.tiModal.open(IntelSpeedAdjustComponent, {
            id: 'intelSpeedAdjustId',
            modalClass: 'fanModal',
            context: {
                intelSpeedAdjust: this.intelSpeedAdjust,
                param: null
            },
            close: (modalRef: TiModalRef): void => {
                const param = modalRef.content.instance.param;
                if (param) {
                    this.service.intelligentUpdate(param).subscribe((response) => {
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        const intelSpeedAdjustInfo = this.service.getIntelSpeedAdjustInfo(response['body']);
                        this.initializingIntelSpeedAdjustInfo(intelSpeedAdjustInfo);
                    }, (error) => {
                        const intelSpeedAdjustInfo = this.service.getIntelSpeedAdjustInfo(error.error.data);
                        this.initializingIntelSpeedAdjustInfo(intelSpeedAdjustInfo);
                    });
                }
            },
            dismiss: (): void => {
            }
        });
    }

    private initializingIntelSpeedAdjustInfo(intelSpeedAdjustInfo) {
        if (intelSpeedAdjustInfo) {
            this.intelSpeedAdjust.setFanSmartCoolingMode(intelSpeedAdjustInfo.fanSmartCoolingMode);
            this.intelSpeedAdjust.setFanSmartCoolingModeText(intelSpeedAdjustInfo.fanSmartCoolingModeText);
            this.intelSpeedAdjust.setCoolingMedium(intelSpeedAdjustInfo.coolingMedium);
            this.intelSpeedAdjust.setCPUTargetTemperatureCelsius(intelSpeedAdjustInfo.cpuTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMinCPUTargetTemperatureCelsius(intelSpeedAdjustInfo.minCPUTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMaxCPUTargetTemperatureCelsius(intelSpeedAdjustInfo.maxCPUTargetTemperatureCelsius);
            this.intelSpeedAdjust.setHbmTargetTemperatureCelsius(intelSpeedAdjustInfo.npuHbmTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMinHbmTargetTemperatureCelsius(intelSpeedAdjustInfo.minHdmTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMaxHbmTargetTemperatureCelsius(intelSpeedAdjustInfo.maxHdmTargetTemperatureCelsius);
            this.intelSpeedAdjust.setAiTargetTemperatureCelsius(intelSpeedAdjustInfo.npuAiCoreTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMinAiTargetTemperatureCelsius(intelSpeedAdjustInfo.minAiTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMaxAiTargetTemperatureCelsius(intelSpeedAdjustInfo.maxAiTargetTemperatureCelsius);
            this.intelSpeedAdjust.setOutletTargetTemperatureCelsius(intelSpeedAdjustInfo.outletTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMinOutletTargetTemperatureCelsius(intelSpeedAdjustInfo.minOutletTargetTemperatureCelsius);
            this.intelSpeedAdjust.setMaxOutletTargetTemperatureCelsius(intelSpeedAdjustInfo.maxOutletTargetTemperatureCelsius);
            this.intelSpeedAdjust.setTemperatureRangeCelsius(intelSpeedAdjustInfo.temperatureRangeCelsius);
            this.intelSpeedAdjust.setFanSpeedPercents(intelSpeedAdjustInfo.fanSpeedPercents);
            this.intelSpeedAdjust.setMinTemperatureRangeCelsius(intelSpeedAdjustInfo.minTemperatureRangeCelsius);
            this.intelSpeedAdjust.setMaxTemperatureRangeCelsius(intelSpeedAdjustInfo.maxTemperatureRangeCelsius);
            this.intelSpeedAdjust.setMinFanSpeedPercents(intelSpeedAdjustInfo.minFanSpeedPercents);
            this.intelSpeedAdjust.setMaxFanSpeedPercents(intelSpeedAdjustInfo.maxFanSpeedPercents);
            this.intelSpeedAdjust.setFanSpeedCustom(true);
        } else {
            this.intelSpeedAdjust.setFanSpeedCustom(false);
        }
    }

    public init() {
        this.service.getThermals().subscribe((response) => {
            const airInletInfo = this.service.getAirInletTemperature(response.body);
            this.airInletTemperature = new AirInletTemperature(
                airInletInfo.inletTemperature,
                airInletInfo.time,
                airInletInfo.inletTempCelsius
            );
            this.fanInfoList = getFanInfo(response.body, this.translate);
            const intelSpeedAdjustInfo = this.service.getIntelSpeedAdjustInfo(response.body);
            this.initializingIntelSpeedAdjustInfo(intelSpeedAdjustInfo);
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }
	ngOnInit(): void {
        this.loading.state.next(true);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.fanModal) {
                this.fanModal.dismiss();
            }
        });
        this.init();
	}
}
