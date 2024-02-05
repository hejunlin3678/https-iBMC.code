import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { formatEntry } from 'src/app/common-app/utils/common';
@Component({
    selector: 'app-config-recovery',
    templateUrl: './config-recovery.component.html',
    styleUrls: ['./config-recovery.component.scss']
})
export class ConfigRecoveryComponent implements OnInit {

    constructor(
        private translate: TranslateService
    ) { }

    @Input() columns;
    @Input() swapData;
    @Input() rowData;
    @Input() backupTime;
    @Input() param;
    public recoverychecked: boolean = false;
    public contentTip: string = '';
    public slotSwi: string = '';
    public selectedTime = [];
    public selectedSwi = [];
    ngOnInit(): void {
        this.getParm();
        this.swapData.forEach(item => {
            item.radioId = '';
        });
        this.slotSwi = this.rowData.slotSwi + '/' + this.rowData.planeType;
        this.contentTip = formatEntry(this.translate.instant('EMM_SWAP_WARN_INFO_TIME'), [this.backupTime]);
    }
    public trackByFn(index: number, item: any): number {
        return item.id;
    }
    // 其他交换配置
    public radioChange(row, index) {
        let newTime = this.backupTime;
        this.slotSwi = this.rowData.slotSwi + '/' + this.rowData.planeType;
        const newSlot = row.slotSwi + '/' + row.planeType;
        this.selectedTime[index] = row.radioId;
        this.selectedSwi[index] = newSlot;
        // 拼接选择的平面和时间
        this.selectedSwi.forEach((item) => {
            if (item) {
                this.slotSwi += ',' + item;
            }
        });
        this.selectedTime.forEach((item) => {
            if (item) {
                newTime += ',' + item;
            }
        });
        this.contentTip = formatEntry(this.translate.instant('EMM_SWAP_WARN_INFO_TIME'), [newTime]);
        this.getParm();
    }
    // 配置恢复参数
    private getParm() {
        this.param = [{
            'SlotId': this.rowData.slotNo,
            'SwitchModel': this.rowData.boardType,
            'FruId': this.rowData.planeType === 'Base' ? 1 : 2,
            'BackupTime': this.backupTime ? this.backupTime : 'current'
        }];
        if (this.recoverychecked && this.selectedTime.length > 0) {
            this.swapData.forEach(item => {
                if (item.radioId) {
                    this.param.push({
                        'SlotId': item.slotNo,
                        'SwitchModel': item.boardType,
                        'FruId': item.planeType === 'Base' ? 1 : 2,
                        'BackupTime': item.radioId ? item.radioId : 'current'
                    });
                }
            });
        }
    }
    close(): void {
    }
    dismiss(): void {
    }

}
