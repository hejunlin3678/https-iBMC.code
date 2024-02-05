import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';
import { Span } from './span.model';

export class SpanNode implements TiTreeNode {

    id: string;
    label: string;
    url: string;
    children: TiTreeNode[];
    checked: boolean | string;
    active: boolean;
    disabled: boolean;
    labelId: string;
    readonly componentName: string = 'SpanNode';
    private span: Span;
    private raidIndex: number;

    constructor(lable: string, url: string, id: string, raidIndex: number, labelId: string) {
        this.id = id;
        this.label =  lable;
        this.url = url;
        this.checked = false;
        this.disabled = false;
        this.raidIndex = raidIndex;
        this.labelId = labelId;
    }

    addChildren(children: TiTreeNode): void {
        if (!this.children) { this.children = []; }
        this.children.push(children);
    }

    set setChildren(children: TiTreeNode[]) {
        this.children = children;
    }

    get getRaidIndex(): number {
        return this.raidIndex;
    }

    get getUrl(): string {
        return this.url;
    }

    set setSpan(span: Span) {
        this.span = span;
    }

    get getSpan(): Span {
        return this.span;
    }

    get getLabel(): string {
        return this.label;
    }

}
