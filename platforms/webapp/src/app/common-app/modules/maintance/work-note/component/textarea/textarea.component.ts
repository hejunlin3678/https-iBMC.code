import { Component, OnInit, Input, EventEmitter, Output, SimpleChanges } from '@angular/core';
import { UntypedFormControl, ValidatorFn } from '@angular/forms';
import { getByte, cutStr } from 'src/app/common-app/utils';
import { SystemLockService } from 'src/app/common-app/service';

@Component({
    selector: 'app-textarea',
    templateUrl: './textarea.component.html',
    styleUrls: ['./textarea.component.scss']
})
export class TextareaComponent implements OnInit {

    @Input() maxCount: number;

    @Input() initContent: string = '';

    // 是否清空输入框内容
    @Input() initValueFlag: boolean = false;

    @Input() disabled: boolean = false;

    @Input() control: ValidatorFn[];

    @Input() placeholder: string = 'WORK_NOTE_DETAIL_PLACEHOLDER';

    @Output() contentChange = new EventEmitter();

    public content: string = '';
    public systemLocked: boolean = true;
    // 右下角剩余字节数
    public extraSize: string;

    public formControl: UntypedFormControl;

    constructor(
        private lockService: SystemLockService
    ) { }

    ngOnInit() {
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
    }

    public textChange(str: string) {
        const currentSize = this.setValue(str);
        if (currentSize > this.maxCount) {
            // 修改自己的内容需要是异步操作
            setTimeout(() => {
                const currentValue = cutStr(str, this.maxCount);
                if (this.formControl) {
                    this.formControl.setValue(currentValue);
                }
                this.setValue(currentValue);
            }, 0);
        }
    }

    private showExtraSize() {
        const currentSize = getByte(this.content);
        this.extraSize = String(currentSize);
    }

    private setValue(str: string) {
        this.content = str;
        const currentSize = getByte(str);
        this.extraSize = String(currentSize);
        let isError = false;
        if (this.control) {
            isError = Boolean(this.formControl.errors);
        }
        this.contentChange.emit({
            value: this.content,
            isError,
        });
        return currentSize;
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.initValueFlag) {
            this.setValue(this.initContent);
        }
        if (changes.control) {
            this.formControl = new UntypedFormControl(this.initContent, changes.control.currentValue);
        }
        if (changes.disabled && this.control) {
            if (changes.disabled.currentValue) {
                this.formControl.disable();
            } else {
                this.formControl.enable();
            }
        }
        this.showExtraSize();
    }

}
