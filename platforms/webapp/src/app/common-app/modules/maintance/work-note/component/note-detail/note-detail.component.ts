import { Component, OnInit, Input, SimpleChanges, Output, EventEmitter, ElementRef, ViewChild } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { Subscription } from 'rxjs';
import { TiMessageService } from '@cloud/tiny3';
import { UserInfoService, AlertMessageService } from 'src/app/common-app/service';
import { IRecord } from '../../work-note.datatype';
import { WorkNoteService } from '../../services';

@Component({
    selector: 'app-note-detail',
    templateUrl: './note-detail.component.html',
    styleUrls: ['./note-detail.component.scss']
})
export class NoteDetailComponent implements OnInit {

    @ViewChild('target', { static: true }) target: ElementRef;

    @Input() record: IRecord;
    @Input() systemLockDownEnabled;

    @Output() updateRecord = new EventEmitter();

    @Output() editRecord = new EventEmitter();


    public textareaValue: string;

    private deleteSub: Subscription;

    public edit() {
        if (this.systemLockDownEnabled) {
            return;
        }
        this.editRecord.emit({
            id: this.record.id,
            target: this.target.nativeElement,
            textareaValue: this.textareaValue,
        });
    }

    public delete() {
        this.deleteSub = this.workNoteServ.deleteRecord(this.record.id).subscribe((res) => {
            if (res['@Message.ExtendedInfo']) {
                this.alertServ.eventEmit.emit({
                    type: 'error',
                    label: 'COMMON_FAILED'
                });
            } else {
                this.alertServ.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });

                this.updateRecord.emit();
            }
        });
    }

    public showDeleteModel() {
        if (this.systemLockDownEnabled) {
            return;
        }

        const messageInstance = this.tiMessage.open({
            id: `deleteModel${this.record.id}`,
            closeIcon: false,
            type: 'warn',
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    messageInstance.close();
                    this.delete();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_CANCEL'),
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
        });

    }

    constructor(
        private userService: UserInfoService,
        private workNoteServ: WorkNoteService,
        private alertServ: AlertMessageService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
    ) { }

    ngOnInit() {
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.record) {
            this.textareaValue = this.record.details;
        }
    }

    ngOnDestroy(): void {
        if (this.deleteSub && this.deleteSub.unsubscribe) {
            this.deleteSub.unsubscribe();
        }
    }

}
