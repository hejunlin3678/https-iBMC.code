import { Component, OnInit } from '@angular/core';
import { SystemLogService } from './services';
import { LoadingService, UserInfoService } from 'src/app/common-app/service';
import * as utils from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-system-log',
    templateUrl: './system-log.component.html',
    styleUrls: ['./system-log.component.scss']
})
export class SystemlogComponent implements OnInit {
    public npuLogProgressUrl: string = sessionStorage.getItem('npuLogProgressUrl');
    public npuLogProgressItem: any = JSON.parse(sessionStorage.getItem('npuLogProgressItem'));
    constructor(
        private service: SystemLogService,
        private loading: LoadingService,
        private translate: TranslateService,
        private user: UserInfoService
    ) { }
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    // systemInfo第二个待判断产品类型更换desc描述
    public systemInfo = [{
        id: 'blackBox',
        title: 'SYSTEM_LOG_BLACKCLIP',
        idPCIe: 'blackBoxPCIe',
        titlePCIe: 'SYSTEM_LOG_PCIE',
        statusPCIe: false,
        disable: false,
        desc: 'SYSTEM_LOG_BLACKCLIP1',
        status: false,
        fileName: 'blackbox.tar',
        size: '4M',
        down: false,
        hideMctp: false,
        blackSdi: false,
        fileNameSid: 'sdi_blackbox.tar',
        titleSDI: 'SYSTEM_LOG_SDI',
        sizeSid: '16M',
        downSid: false,
        showSdiProgress: false,
    },
    {
        id: 'serialPort',
        title: 'SYSTEM_LOG_SERIAL_PORT',
        desc: 'SYSTEM_LOG_SERIAL_PORT1_TCE',
        status: false,
        fileName: 'systemcom.tar',
        size: '2M',
        down: false,
        disable: false
    }];
    public npuCards = [];
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    // 获取处理器集合
    public getAllProcessor() {
        this.service.getAllProcessor().subscribe((res) => {
            const response = res.body;
            if (response.NPU) {
               this.npuCards = this.parsingInfo(response.NPU);
            }
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }

    public parsingInfo(mebers: object[]): object[] {
        const tempS = [];
        mebers.forEach((item) => {
            tempS.push(this.parsingNPUInfo(item));
        });
        return tempS;
    }

    public parsingNPUInfo (npu: any) {
        let temp = {
            fileName: 'npu.tar.gz',
            status: true,
            title: '',
            id: 'NPU',
            down: false,
            showProgress: false,
            isCollectingOk: false,
            isCollectingErr: false,
        };
        if (npu.State !== 'Absent') {
            temp.fileName = 'npu.tar.gz',
            temp.title = npu.Name;
            temp.id = 'NPU',
            temp.status =  true;
            temp.down = false;
            temp.showProgress = false;
            temp.isCollectingOk = false;
            temp.isCollectingErr = false;
            if (this.npuLogProgressUrl && this.npuLogProgressItem && this.npuLogProgressItem.title === temp.title) {
                temp = this.npuLogProgressItem;
            }
            return temp;
        }
    }

    ngOnInit(): void {
        const productName = sessionStorage.getItem('productName');
        this.loading.state.next(true);
        this.getAllProcessor();
        this.service.switchState().subscribe((res) => {
            const response = res.body;
            // 黑匣子
            const blackBoxEnabled = response.BlackBoxEnabled;
            // 串口
            const serialDataEnabled = response.SerialPortDataEnabled;
            this.systemInfo[0].status = blackBoxEnabled;
            this.systemInfo[1].status = serialDataEnabled;
            // 天池MCTP功能已裁剪，不展示MCTP信息（MctpSuported返回true就是天池）
            this.systemInfo[0].hideMctp = response?.MctpSuported;
            // PCIe接口
            const pcieInterfaceEnabled = response.PCIeInterfaceEnabled;
            this.systemInfo[0].statusPCIe = pcieInterfaceEnabled;
            if (!this.systemInfo[0].statusPCIe) {
                this.systemInfo[0].disable = true;
            }
            // 此处需要根据产品名称修改大小
            const blackBoxSize = Math.round(response.BlackBoxSize / (1024 * 1024));
            const serialDataSize = Math.round(response.SerialPortDataSize / (1024 * 1024));
            this.systemInfo[0].size = blackBoxSize + 'M';
            this.systemInfo[1].size = serialDataSize + 'M';
            const desc = utils.formatEntry(this.translate.instant('SYSTEM_LOG_SERIAL_PORT1'), [serialDataSize]);
            this.systemInfo[1].desc = desc;

            // 是否支持SDI
            this.systemInfo[0].blackSdi = response.IsSupportBlackBoxOfSdi;

            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }
}
