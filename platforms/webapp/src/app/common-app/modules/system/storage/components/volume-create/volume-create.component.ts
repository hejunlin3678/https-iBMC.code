import { Component, OnInit, DoCheck, Output, EventEmitter } from '@angular/core';
import { VolumeCreateService } from './volume-create.service';
import { map } from 'rxjs/internal/operators/map';
import { Observable } from 'rxjs/internal/Observable';
import { StorageTree, RaidNode, VolumeCreateModel } from '../../models';
import { RaidService } from '../raid/raid.service';
import { share } from 'rxjs/internal/operators/share';
import { AlertMessageService } from 'src/app/common-app/service';
import { LoadingService } from 'src/app/common-app/service';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { getMessageId, errorTip } from 'src/app/common-app/utils';


@Component({
    selector: 'app-volume-create',
    templateUrl: './volume-create.component.html',
    styleUrls: ['./volume-create.component.scss']
})
export class VolumeCreateComponent implements OnInit, DoCheck {

    private selectNode: RaidNode;
    @Output() close: EventEmitter<any> = new EventEmitter<any>();

    public volumeCreate: VolumeCreateModel;

    public raidNode$: Observable<RaidNode>;

    constructor(
        private volumeCreateService: VolumeCreateService,
        private raidService: RaidService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
    ) {
        this.volumeCreate = VolumeCreateModel.getInstance();
    }
    ngDoCheck(): void {
        this.volumeCreate.checkBtnState();
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        this.selectNode = StorageTree.getInstance().getCheckedNode as RaidNode;
        const getRaidPost$ = this.raidService.factory(this.selectNode).pipe(
            map((raid) => {
                this.loading.state.next(false);
                this.selectNode.setRaid = raid;
                return this.selectNode;
            })
        );
        this.volumeCreateService.raidNode$ = getRaidPost$.pipe(share());
    }
    public sure(): void {
        const msgInstance = this.tiMessage.open({
            id: 'driverCreatMessage',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    msgInstance.close();
                    this.loading.state.next(true);
                    this.volumeCreateService.createVolumn().subscribe(
                        () => {
                            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });

                        },
                        (error) => {
                            this.showErrorTip(error);
                        },
                        () => {
                            this.loading.state.next(false);
                            this.close.next(true);
                        }
                    );
                }
            },
            cancelButton: {
                text: this.translate.instant('COMMON_CANCEL')
            }
        });
    }

    public cancel(): void {
        this.close.next(false);
    }

    private showErrorTip(error: any): void {
        const msgId = error.error ? getMessageId(error.error)[0].errorId : 'COMMON_FAILED';
        this.alert.eventEmit.emit(errorTip(msgId, this.translate));
        this.loading.state.next(false);
    }
}
