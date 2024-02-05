import { Component, EventEmitter, Input, Output, ViewChild } from '@angular/core';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';
import { IHalfModalContent } from '../../work-note.datatype';

@Component({
    selector: 'app-note-half-modal',
    templateUrl: './base-note-half-modal.component.html',
    styleUrls: ['./base-note-half-modal.component.scss']
})

export class BaseNoteHalfModalComponent {

    @ViewChild('modal', { static: true }) halfmodal: TpHalfmodalComponent;

    @Input() content: IHalfModalContent;

    @Input() initTextareaValue: string = '';

    @Input() initValueFlag: boolean;

    @Output() triggerSave = new EventEmitter();

    @Output() triggerCancel = new EventEmitter();

    @Output() transmitHalfModal = new EventEmitter();

    public isEdit = false;

    public readonly MAX_COUNT = 255;

    public buttonDisable: boolean = true;

    public textareaValue: string;

    // 阻止点击时冒泡导致半屏弹窗关闭
    public stopBubble(event) {
        event.stopPropagation();
    }

    public cancel() {
        this.triggerCancel.emit();
    }

    public confirm() {

    }

    public save() {

    }

    public textareaChange({ value, isError }) {
        this.textareaValue = value;
        const notChanged = value === this.initTextareaValue;
        this.buttonDisable = !Boolean(this.textareaValue) || notChanged || isError;
    }
}
