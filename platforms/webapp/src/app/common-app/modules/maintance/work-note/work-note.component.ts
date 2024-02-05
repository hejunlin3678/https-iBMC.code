import { Component, OnInit, ElementRef, ViewChild } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { Subscription } from 'rxjs';
import { WorkNoteService } from './services';
import { LoadingService, UserInfoService } from 'src/app/common-app/service';
import { IHalfModalContent, IRecord } from './work-note.datatype';
import { TiMessageService } from '@cloud/tiny3';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';

@Component({
    selector: 'app-work-note',
    templateUrl: './work-note.component.html',
    styleUrls: ['./work-note.component.scss']
})
export class WorknoteComponent implements OnInit {

    @ViewChild('target', { static: true }) target: ElementRef;

    private halfModal: TpHalfmodalComponent;

    public addHalfmodal: TpHalfmodalComponent;

    public editHalfmodal: TpHalfmodalComponent;

    // 将textarea子组件内容初始化
    public initValueFlag: boolean = true;

    public systemLockDownEnabled = this.userService.systemLockDownEnabled;

    public halfModalContent: IHalfModalContent = {
        currentIP: '',
        userName: this.userService.userName,
        systemLockDownEnabled: this.systemLockDownEnabled,
    };
    private getRecordSub: Subscription;

    private getSessionSub: Subscription;

    public currentEditId: string;

    public records: IRecord[] = [];

    // 最多20条记录
    private readonly MAX_ACCORDS = 20;

    public textareaValue = '';

    public initTextareaValue = '';

    public loaded: boolean;

    public async getRecord() {
        await new Promise((resolve) => {
            this.getRecordSub = this.workNoteServ.getData().subscribe((records) => {
                this.records = records;
                resolve(true);
            }, () => {
                resolve(false);
            });
        });
    }

    private async getIP() {
        await new Promise((resolve) => {
            this.getSessionSub = this.workNoteServ.getIp().subscribe((ip) => {
                this.halfModalContent.currentIP = ip;
                resolve(true);
            }, () => {
                resolve(false);
            });
        });

    }

    // 因半屏弹窗组件需要从子组件获取，所以将半屏弹窗传递给父组件
    public initAddModal(modal: TpHalfmodalComponent) {
        this.addHalfmodal = modal;
    }

    public initEditModal(modal: TpHalfmodalComponent) {
        this.editHalfmodal = modal;
    }

    public editOrSaveNote() {
        this.halfModal.hide();
        this.getRecord();
    }

    public cancel() {
        this.halfModal.hide();
    }

    public editRecord({ id, target, textareaValue }) {
        this.currentEditId = id;
        this.textareaValue = this.initTextareaValue = textareaValue;
        this.halfModal = this.editHalfmodal;
        this.initValueFlag = !this.initValueFlag;
        this.halfModal.show(target);
    }

    public showHalfModal() {
        if (this.records && this.records.length >= this.MAX_ACCORDS) {
            this.tiMessage.open({
                id: 'maxNoteMsg',
                type: 'prompt',
                closeIcon: false,
                okButton: {
                    text: this.translate.instant('COMMON_OK'),
                    autofocus: false,
                },
                cancelButton: {
                    show: false
                },
                title: this.translate.instant('COMMON_INFORMATION'),
                content: this.translate.instant('WORK_NOTE_CONFIRM_CONTENT_NOMORE_DATA')
            });
            return;
        }
        this.halfModal = this.addHalfmodal;
        this.initValueFlag = !this.initValueFlag;
        this.halfModal.show(this.target.nativeElement);
    }

    constructor(
        private translate: TranslateService,
        private workNoteServ: WorkNoteService,
        private loadingServ: LoadingService,
        private userService: UserInfoService,
        private tiMessage: TiMessageService,
    ) { }

    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.systemLockDownEnabled !== state) {
            this.systemLockDownEnabled = state;
        }
    }
    async ngOnInit() {
        this.loadingServ.state.next(true);
        this.loaded = false;

        const getRecordPromise = this.getRecord();
        const getIPPromise = this.getIP();
        await getRecordPromise;
        await getIPPromise;

        this.loadingServ.state.next(false);
        this.loaded = true;
    }

    ngOnDestroy(): void {
        this.getRecordSub.unsubscribe();
        this.getSessionSub.unsubscribe();
    }

}
