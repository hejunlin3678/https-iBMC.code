import { Component, Input, EventEmitter, Output, TemplateRef, ChangeDetectorRef } from '@angular/core';

import { TranslateService } from '@ngx-translate/core';
import { TiModalService, TiTableRowData } from '@cloud/tiny3';

@Component({
    selector: 'app-event-table',
    templateUrl: './event-table.component.html',
    styleUrls: ['./event-table.component.scss']
})
export class EventTableComponent {

    constructor(private translate: TranslateService, private tiModal: TiModalService, private changeRef: ChangeDetectorRef) { }
    @Input() type;
    @Input() columns;
    @Input() srcData;
    @Input() currentPage;
    @Input() pageSize;
    @Input() totalNumber;
    @Output() curuentPageUpdate = new EventEmitter<number>();
    public contentDetail: string = '';
    public displayed: TiTableRowData[] = [];

    public pageUpdate(page) {
        this.curuentPageUpdate.emit(page);
    }
    // 打开详情弹窗
	public openDeatils(row: object, modalTemplate: TemplateRef<any>) {
        this.contentDetail = row['eventDetails'];
        this.tiModal.open(modalTemplate, {
            id: 'myModal',
            /**
             * 模板中实际调用的是Modal服务提供的close和dismiss方法
             * 此处定义close和dismiss方法只是为了避免生产环境打包时报错
             */
            close: (): void => {
            },
            dismiss: (): void => {
            }
        });
	}
}
