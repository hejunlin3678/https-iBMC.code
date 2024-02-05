import { Component, AfterViewInit } from '@angular/core';
import { FdmService } from '../../fdm.service';
import { FDMNode, DeviceCase } from '../../classes';
import { DeviceCaseService } from './device-case.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service/loading.service';

@Component({
    selector: 'app-device-case',
    templateUrl: './device-case.component.html',
    styleUrls: ['./device-case.component.scss']
})
export class DeviceCaseComponent implements AfterViewInit {

    public treeNode: FDMNode;
    public deviceCase: DeviceCase;
    constructor(
        private fdmService: FdmService,
        private deviceCaseService: DeviceCaseService,
        private loading: LoadingService,
        private i18n: TranslateService
    ) {
        this.deviceCase = new DeviceCase(i18n, []);
    }
    ngAfterViewInit() {
        this.fdmService.activeNode$.subscribe(
            (node: FDMNode) => {
                if (node) {
                    this.loading.state.next(true);
                    this.deviceCaseService.factory(node).subscribe(
                        (result) => {
                            this.deviceCase = result;
                            this.loading.state.next(false);
                        },
                        () => {},
                        () => {
                            this.loading.state.next(false);
                        }
                    );
                }
            }
        );
    }
    public changeType($event) {
        this.deviceCase.setType($event);
    }
    public changeLevel($event) {
        this.deviceCase.setLevel($event);
    }
    public ngModelChange($event) {
        this.deviceCase.setTime($event);
    }
}
