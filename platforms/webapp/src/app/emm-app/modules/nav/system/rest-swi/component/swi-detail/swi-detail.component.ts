import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { LoadingService } from 'src/app/common-app/service';
import { RestSwiService } from '../../services/rest-swi.service';
import { htmlEncode } from 'src/app/common-app/utils/common';

@Component({
    selector: 'app-swi-detail',
    templateUrl: './swi-detail.component.html',
    styleUrls: ['./swi-detail.component.scss']
})
export class SwiDetailComponent implements OnInit {
    constructor(
        private loading: LoadingService,
        private restSwiService: RestSwiService
    ) { }
    @Input() row;
    @Input() backupTime;
    @Input() halfTitle: string;
    @Output() close: EventEmitter<boolean> = new EventEmitter<boolean>();
    public disabled: boolean = false;
    public detailInfo: string = '';
    ngOnInit(): void {
        this.init();
    }
    public init() {
        this.loading.state.next(true);
        if (this.backupTime === undefined) {
            this.disabled = true;
        }
        const param: { [key: string]: any } = {};
        param['SlotId'] = this.row.slotNo;
        param['SwitchModel'] = this.row.boardType;
        param['FruId'] = this.row.planeType === 'Base' ? 1 : 2;
        param['BackupTime'] = this.backupTime ? this.backupTime : 'current';
        // 此详情接口状态200但是会走到错误方法，因此从错误方法获取的内容
        this.restSwiService.getDetailInfo(param).subscribe((response) => {
            this.loading.state.next(false);
            this.detailInfo = response;
        }, (error) => {
            this.loading.state.next(false);
            const detailText = htmlEncode(error['error']?.text);
            this.detailInfo = detailText.replace(/\n/g, '<br>').replace(/\s/g, '&nbsp;');
        });
    }
    public save() {
        this.close.next(true);
    }
    // 关闭半屏弹窗
    public closeModal() {
        this.close.next(false);
    }

}

