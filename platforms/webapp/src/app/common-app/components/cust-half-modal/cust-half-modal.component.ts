import { Component, OnInit, ViewChild, ElementRef, Output, EventEmitter, Input, SimpleChanges } from '@angular/core';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { DomSanitizer } from '@angular/platform-browser';

@Component({
    selector: 'app-cust-half-modal',
    templateUrl: './cust-half-modal.component.html',
    styleUrls: ['./cust-half-modal.component.scss']
})
export class CustHalfModalComponent implements OnInit {
    @Output() close: EventEmitter<any> = new EventEmitter<any>();
    @Output() save: EventEmitter<any> = new EventEmitter<any>();
    @Input() disabled: boolean = true;
    @Input() state: boolean = false;
    @Input() title: string = '';
    @Input() scrollTop: number = null;
    @Input() saveText: string = '';
    public userLoginPwd: string = '';
    public saveError: boolean = false;
    public systemLocked: boolean = false;
    public showFooter: boolean = false;
    public saveTextBtn: string = 'COMMON_SAVE';

    @ViewChild(TpHalfmodalComponent, { static: true }) modal: TpHalfmodalComponent;
    @ViewChild('target', { static: true }) target: ElementRef;
    constructor(
        private lockService: SystemLockService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private domSanitizer: DomSanitizer
    ) { }

    ngOnInit(): void {
        // 系统锁定
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe((res: boolean) => {
            this.systemLocked = res;
        });
        if (this.saveText) {
            this.saveTextBtn = this.saveText;
        }
        this.openModal();
    }

    public openModal(): void {
        // 使用延时的原因是因为变更检测会提示数据已经发生变化了
        setTimeout(() => {
            this.modal.show();
            this.showFooter = true;
        }, 0);
    }

    // 保存按钮点击事件
    public saveData(): void {
        this.save.emit(true);
    }

    // 关闭半屏弹窗，并通知外层组件重刷数据
    private destroyModal(): void {
        this.modal.hide();
        this.close.emit({ reload: true });
    }

    // 取消按钮点击事件
    public closeModal(): void {
        const msg = `<span id="confirmText" class="font-14">${this.translate.instant('COMMON_ASK_OK')}</span>`;
        this.tiMessage.open({
            id: 'secondConfirm',
            type: 'prompt',
            backdrop: false,
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.domSanitizer.bypassSecurityTrustHtml(msg),
            okButton: {
                text: this.translate.instant('COMMON_OK'),
                autofocus: false
            },
            close: () => {
                // 点击确认时
                this.destroyModal();
            }
        });
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.state && changes.state.currentValue) {
            this.destroyModal();
        }

        if (changes.scrollTop && changes.scrollTop.currentValue !== null) {
            this.modal.nativeElement.scrollTop = `${changes.scrollTop.currentValue}px`;
        }
    }

    ngOnDestroy(): void {
        this.modal.hide();
    }
}
