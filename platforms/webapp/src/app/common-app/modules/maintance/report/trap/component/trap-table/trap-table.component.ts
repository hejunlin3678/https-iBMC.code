import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { TiTableRowData, TiMessageService } from '@cloud/tiny3';
import { AlertMessageService, LoadingService, ErrortipService } from 'src/app/common-app/service';
import { TrapService } from '../../service';
import { getMessageId } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-trap-table',
    templateUrl: './trap-table.component.html',
    styleUrls: ['./trap-table.component.scss']
})
export class TrapTableComponent implements OnInit {

    constructor(
        private service: TrapService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private loading: LoadingService
    ) { }
    public displayed: TiTableRowData = [];
    public sampleObj = {
        Time: ['Time', '1970-01-01 01:52:00'],
        SensorName: ['Sensor', 'test sensor'],
        Severity: ['Severity', 'Assertion Normal'],
        EventCode: ['Code', '0x00000001'],
        EventDescription: ['Description', 'sensor test event']
    };
    @Input() srcData;
    @Input() columns;
    @Input() renderData;
    @Input() hasPermission;
    @Input() isSystemLock;
    @Input() btnSaveState;
    @Input() isIRM210;
    @Output() operateChange = new EventEmitter();
    public currentRow = {
        separator: '',
        keyWord: false,
        content: [],
        example: []
    };

    ngOnInit() {
    }
    public toggle(row) {
        row.showDetails = !row.showDetails;
        row.isShowDetails = false;
        row.isEdit = false;
    }
    public edit(row) {
        if (this.isSystemLock || !this.hasPermission) {
            return;
        }
        row.showDetails = true;
        row.isEdit = true;
        row.isShowDetails = true;
        this.currentRow.separator = row.separator;
        this.currentRow.keyWord = row.keyWord;
        this.currentRow.content = this._copy(row.reportContent);
        this.currentRow.example = this._copySingleArray(row.example);
    }
    public cancel(row) {
        row.showDetails = !row.showDetails;
        row.isEdit = false;
        row.status = row.statusBak;
        row.port = row.portBak;
        row.serverAddr = row.serverAddrBak;
        row.trans = row.transBak;
        row.separator = this.currentRow.separator;
        row.keyWord = this.currentRow.keyWord;
        row.reportContent = this._copySingleArray(this.currentRow.content);
        row.example = this._copySingleArray(this.currentRow.example);
    }
    public save(row, index) {
        const isValid =
            (row.addrControl.errors ? row.addrControl.errors.pattern.tiErrorMessage : row.addrControl.errors)
            || row.portControl.errors;
        if (isValid) {
            if (row.addrControl.errors ? row.addrControl.errors.pattern.tiErrorMessage : row.addrControl.errors) {
                document.getElementById('serverAddr' + index).focus();
                document.getElementById('serverAddr' + index).blur();
                document.getElementById('serverAddr' + index).focus();
            } else if (row.portControl.errors) {
                document.getElementById('port' + index).focus();
                document.getElementById('port' + index).blur();
                document.getElementById('port' + index).focus();
            }
            return;
        }
        row.disable = true;
        const msgContent = [];
        for (const key of row.reportContent) {
            if (key.checked) {
                msgContent.push(key.id);
            }
        }
        const obj = {
            ID: row.num - 1,
            Enabled: row.status,
            IPAddress: row.serverAddr,
            Port: parseInt(row.port, 10),
            BobEnabled: row.trans,
            MessageDelimiter: row.separator,
            MessageContent: msgContent,
            DisplayKeywords: row.keyWord
        };
        const params = {
            TrapServerList: [obj]
        };
        this.loading.state.next(true);
        this.service.trapSave(params).subscribe((res) => {
            const response = res['body'];
            if (response.error) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                this.cancel(row);
                row.disable = false;
                this.loading.state.next(false);
            } else {
                row.showDetails = false;
                row.isEdit = false;
                row.disable = false;
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.operateChange.emit({
                    key: 'save',
                    value: {
                        TrapMode: response.Mode,
                        TrapServer: response.TrapServerList
                    }
                });
            }
            this.loading.state.next(false);
        }, () => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.cancel(row);
            row.disable = false;
            this.loading.state.next(false);
        });
    }
    // 表格测试
    public test(row) {
        if (!this.hasPermission) {
            return;
        } else if (!this.btnSaveState) {
            const trapTestModel = this.tiMessage.open({
                id: 'trapTestModel',
                type: 'prompt',
                okButton: {
                    show: true,
                    autofocus: false,
                    click() {
                        trapTestModel.close();
                    }
                },
                cancelButton: {
                    show: false
                },
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.translate.instant('ALARM_REPORT_TRAP_TEST')
            });
            return;
        }
        const queryData = {
            ID: row.num - 1
        };
        this.loading.state.next(true);
        this.service.trapSendTest(queryData).subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.loading.state.next(false);
        }, (error) => {
            const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            this.loading.state.next(false);
        });
    }
    // 右边内容修改后
    public change(row) {
        const arr = [];
        const isKey = row.keyWord;
        for (const key of row.reportContent) {
            if (key.checked) {
                for (const k in this.sampleObj) {
                    if (key.id === k) {
                        if (isKey) {
                            arr.push(`${this.sampleObj[k][0]}:  ${this.sampleObj[k][1]}`);
                        } else {
                            arr.push(this.sampleObj[k][1]);
                        }
                    }
                }
            }
        }
        row.example = arr;
    }
    public checkIsEqual(row) {
        const leftIsEqual =
            row.status === row.statusBak
            && row.port === row.portBak
            && row.serverAddr === row.serverAddrBak
            && row.trans === row.transBak;
        if (this.renderData.mode.id === 'EventCode') {
            // 事件码模式下需要判断右边内容
            const rigthIsEqual =
                row.separator === this.currentRow.separator
                && row.keyWord === this.currentRow.keyWord
                && this._checkContent(row.reportContent, this.currentRow.content);
            return leftIsEqual && rigthIsEqual;
        } else {
            return leftIsEqual;
        }
    }
    private _copy(content) {
        const arr = [];
        for (const iterator of content) {
            if (Object.prototype.toString.call(iterator) === '[object Object]') {
                const obj = {};
                for (const key in iterator) {
                    if (iterator.hasOwnProperty(key)) {
                        obj[key] = iterator[key];
                    }
                }
                arr.push(obj);
            }
        }
        return arr;
    }
    private _checkContent(c1, c2) {
        for (const iterator of c1) {
            for (const item of c2) {
                if (iterator.id === item.id) {
                    if (iterator.checked === item.checked) {
                        break;
                    } else {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    private _copySingleArray(array) {
        const arr = [];
        if (array && array.length > 0) {
            for (const iterator of array) {
                arr.push(iterator);
            }
        }
        return arr;
    }
    // Trap端口重置
    public restTrapFN(row) {
        row.port = this.service.trapPort;
    }
}
