import { Component, Input, OnInit, ViewChild, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { BaseNoteHalfModalComponent } from './base-note-half-modal.component';
import { AlertMessageService, SystemLockService } from 'src/app/common-app/service';
import { WorkNoteService } from './../../services/work-note.service';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';
import { IEditRecord } from '../../work-note.datatype';

@Component({
    selector: 'app-edit-note-half-modal',
    templateUrl: './base-note-half-modal.component.html',
    styleUrls: ['./base-note-half-modal.component.scss']
})
export class EditNoteHalfModalComponent extends BaseNoteHalfModalComponent implements OnInit, OnDestroy {

    @ViewChild('modal', { static: true }) halfmodal: TpHalfmodalComponent;

    @Input() currentEditId: string;

    private updateSub: Subscription;

    public isEdit = true;
    public systemLocked: boolean = true;
    public save() {
        const params: IEditRecord = {
            ID: this.currentEditId,
            Content: this.textareaValue
        };
        const update$ = this.workNoteServ.updateRecord(params);
        this.updateSub = update$.subscribe(() => {
            this.alertServ.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.triggerSave.emit();
        });
    }

    constructor(
        private workNoteServ: WorkNoteService,
        private alertServ: AlertMessageService,
        private lockService: SystemLockService
    ) {
        super();
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
        if (this.updateSub && this.updateSub.unsubscribe) {
            this.updateSub.unsubscribe();
        }
    }

}
