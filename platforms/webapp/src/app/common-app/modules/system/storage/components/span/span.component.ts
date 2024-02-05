import { Component, OnInit } from '@angular/core';
import { LoadingService } from 'src/app/common-app/service';
import { SpanService } from './span.service';
import { StorageTree, Span, SpanNode } from '../../models';

@Component({
    selector: 'app-span',
    templateUrl: './span.component.html'
})
export class SpanComponent implements OnInit {

    public span: Span;
    constructor(
        private spanService: SpanService,
        private loading: LoadingService
    ) { }

    ngOnInit() {
        const spanNode = StorageTree.getInstance().getCheckedNode as SpanNode;
        if (spanNode.getSpan) {
            this.span = spanNode.getSpan;
        } else {
            this.loading.state.next(true);
            this.spanService.factory(spanNode).subscribe(
                (node: Span) => {
                    this.span = node;
                    spanNode.setSpan = this.span;
                },
                () => {},
                () => {
                    this.loading.state.next(false);
                }
            );
        }
    }

}
