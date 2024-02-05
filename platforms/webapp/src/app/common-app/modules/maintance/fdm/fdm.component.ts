import { Component, OnDestroy, OnInit } from '@angular/core';
import { FDMNode } from './classes';
import { FdmService } from './fdm.service';
import { BehaviorSubject } from 'rxjs/internal/BehaviorSubject';
import { LoadingService } from 'src/app/common-app/service/loading.service';

@Component({
    selector: 'app-fdm',
    templateUrl: './fdm.component.html',
    styleUrls: ['./fdm.component.scss']
})
export class FdmComponent implements OnInit, OnDestroy {

    public treeNode: FDMNode[];

    constructor(
        private fdmService: FdmService,
        private loading: LoadingService
    ) {
        this.treeNode = [];
    }
    ngOnDestroy(): void {
        this.fdmService.activeNode$ = null;
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        this.fdmService.activeNode$ = new BehaviorSubject<FDMNode>(null);
        this.fdmService.factory().subscribe(
            (results) => {
                this.treeNode = results;
                this.fdmService.treeNode = this.treeNode;
                this.fdmService.activeNode$.next(this.treeNode[0]);
            },
            () => { },
            () => {
                setTimeout(() => {
                    this.loading.state.next(false);
                }, 3000);
            }
        );
    }

    public changeFn(event: FDMNode): void {
        this.fdmService.activeNode$.next(event);
    }

}
