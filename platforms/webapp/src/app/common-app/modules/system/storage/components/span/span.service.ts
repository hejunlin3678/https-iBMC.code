import { Injectable } from '@angular/core';
import { map } from 'rxjs/internal/operators/map';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { getUnitConversionByte } from '../../utils';
import { VolumesService } from '../volumes/volumes.service';
import { VolumeNode, StorageTree, StorageStaticData, SpanNode, Span, Volume } from '../../models';
import { TiTreeNode } from '@cloud/tiny3';

@Injectable({
    providedIn: 'root'
})
export class SpanService {
    constructor(
        private http: HttpService,
        private volumeService: VolumesService
    ) { }

    public static creatSpan(spanData: any[], nodeId: string) {
        const parentVolumn = StorageTree.getInstance().searchNodeById(nodeId) as VolumeNode;
        if (spanData && spanData.length > 0) {
            spanData.forEach(
                (data) => {
                    const name = data.ArrayID === 0 || data.ArrayID ? data.ArrayID : StorageStaticData.isEmpty;
                    const usedSpace = getUnitConversionByte(data.UsedSpaceMiB * 1024 * 1024);
                    const remainingSpace = getUnitConversionByte(data.FreeSpaceMiB * 1024 * 1024);
                    if (data.SpanName) {
                        const spanNode = parentVolumn.children.filter((node: TiTreeNode) => node.label === data.SpanName)[0] as SpanNode;
                        spanNode.setSpan = new Span(name, usedSpace, remainingSpace);
                    }
                }
            );
        }
    }

    factory(node: SpanNode): Observable<Span> {
        const parentVolumn = StorageTree.getInstance().getCheckedParent(node) as VolumeNode;
        return this.volumeService.factory(parentVolumn).pipe(
            map(
                (volume: {volume: Volume, isCache: boolean}) => {
                    parentVolumn.setVolume = volume.volume;
                    const spanNode = parentVolumn.children.filter((child) => child.id === node.id)[0] as SpanNode;
                    return spanNode.getSpan;
                }
            )
        );
    }
}
