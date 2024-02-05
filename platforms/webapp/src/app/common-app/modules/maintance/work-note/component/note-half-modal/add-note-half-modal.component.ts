import { Component, OnInit, ViewChild } from '@angular/core';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';
import { Subscription } from 'rxjs';
import { AlertMessageService, SystemLockService } from 'src/app/common-app/service';
import { IAddRecord } from '../../work-note.datatype';
import { WorkNoteService } from './../../services/work-note.service';
import { BaseNoteHalfModalComponent } from './base-note-half-modal.component';

@Component({
    selector: 'app-add-note-half-modal',
    templateUrl: './base-note-half-modal.component.html',
    styleUrls: ['./base-note-half-modal.component.scss']
})
export class AddNoteHalfModalComponent extends BaseNoteHalfModalComponent implements OnInit {
    constructor(
        private alertServ: AlertMessageService,
        private workNoteServ: WorkNoteService,
        private lockService: SystemLockService
    ) {
        super();
    }

    @ViewChild('modal', { static: true }) halfmodal: TpHalfmodalComponent;

    private addNoteSub: Subscription;
    public systemLocked: boolean = true;

    public confirm() {
        const data: IAddRecord = {
            Content: this.textareaValue
        };
        this.addNoteSub = this.workNoteServ.addRecord(data).subscribe(() => {
            this.textareaValue = '';
            this.alertServ.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.triggerSave.emit();
        });
    }

    ngOnInit() {
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
        this.transmitHalfModal.emit(this.halfmodal);
    }

    ngOnDestroy(): void {
        if (this.addNoteSub) {
            this.addNoteSub.unsubscribe();
        }
    }

}
