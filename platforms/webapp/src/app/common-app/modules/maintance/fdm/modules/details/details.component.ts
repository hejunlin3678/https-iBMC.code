import { Component, AfterViewInit } from '@angular/core';
import { FDMNode, FatherObj } from '../../classes';
import { FdmService } from '../../fdm.service';
import { DetailsService } from './details.service';

@Component({
    selector: 'app-details',
    templateUrl: './details.component.html',
    styleUrls: ['./details.component.scss']
})
export class DetailsComponent implements AfterViewInit {

    public treeNode: FDMNode;
    public detail: FatherObj;
    constructor(
        private fdmService: FdmService,
        private detailsService: DetailsService
    ) { }

    ngAfterViewInit() {
        this.fdmService.activeNode$.subscribe(
            (node: FDMNode) => {
                if (node) {
                    this.detailsService.factory(node).subscribe(
                        (result) => {
                            this.detail = result;
                        }
                    );
                }
            }
        );
    }
}
