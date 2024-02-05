import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { DriverNode, VolumeNode, RaidNode, StorageTree, HotSpareNode, SpanNode, RaidType, VolumeStaticModel } from './models';
import { TiTreeNode } from '@cloud/tiny3';
import { of } from 'rxjs/internal/observable/of';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { sortBy } from './utils';

@Injectable({
  providedIn: 'root',
})
export class StorageService {
  public storageTree: StorageTree;

  constructor(private http: HttpService) {}

  public factory() {
    const url = '/UI/Rest/System/Storage/ViewsSummary';
    return this.getData(url).pipe(
      map((data: any) => {
        const stroageTree = StorageTree.getInstance();
        const raidArr = data.RaidControllers;
        if (raidArr && raidArr.length > 0) {
          stroageTree.addTreeNode(this.getRaid(raidArr || []));

          // 生成所有Raid卡的逻辑盘列表
          raidArr.forEach((raid) => {
            const volumes = raid.Volumes;
            if (Array.isArray(volumes) && volumes.length > 0) {
              // 统计maxCache个数是由于只要存在一个maxCache, 则普通逻辑盘容量将无法设置，条带大小无法设置
              let count = 0;
              volumes.forEach((volume) => {
                // 若不是 maxCache盘，则都是普通逻辑盘
                if (volume.SSDCachecadeVolume === true) {
                  count++;
                }
              });
              VolumeStaticModel.logicDriverList[raid.Name] = {};
              VolumeStaticModel.logicDriverList[raid.Name].maxCacheCount =
                count;
            }
          });
        }
        const drives = data.Drives;
        if (drives && drives.length > 0) {
          stroageTree.addTreeNode(this.getDrivers(drives || [], false));
        }
        const bmaEnabled = data?.BmaEnabled;
        const stroageTreeFoliages =
          stroageTree.getFoliages && stroageTree.getFoliages.length > 0
            ? stroageTree
            : null;
        return {
          stroageTreeFoliages,
          bmaEnabled,
        };
      })
    );
  }

  private getRaid(raidArr: any) {
    if (!raidArr || raidArr.length < 1) {
      return null;
    }
    const raidList: RaidNode[] = [];
    raidArr = raidArr.sort(sortBy('Name'));
    raidArr.forEach((item: any, index) => {
      let raidType = '';
      let isSPType = false;
      let specials8iBoard: boolean = false;
      let supportCacheCade = true;
      if (item.Type && String(item.Type)?.indexOf('MSCC') > -1) {
        raidType = RaidType.PMC;
        if (item.Type && String(item.Type) === 'MSCC SmartHBA 2100-8i') {
          raidType = RaidType.HBA;
        }
        // ARIES卡判断逻辑
      } else if (item.Type && String(item.Type)?.startsWith('SP')) {
        raidType = RaidType.ARIES;
      } else {
        raidType = RaidType.BRCM;
        if (item.Type?.includes('9560-8i') || item.Type?.includes('9560-16i')) {
          // 该参数主要针对9560-8i和9560-16i两种类型的板卡raid级别为60的差异处理
          specials8iBoard = true;
        }
      }
      if (item.Type && String(item.Type)?.startsWith('SP686')) {
        isSPType = true;
      }
      if (
        String(item.Type)?.includes('SAS34') ||
        String(item.Type)?.includes('SAS35') ||
        String(item.Type)?.includes('9560')
      ) {
        supportCacheCade = false;
      }
      const raid = new RaidNode(
        item.Name,
        `/UI/Rest/System/Storage/${item.RaidController_ID}`,
        item.RaidController_ID,
        item.VolumeSupported,
        item.RaidController_ID,
        item.EpdSupported,
        item.JbodStateSupported,
        raidType,
        specials8iBoard,
        supportCacheCade
      );
      const volumeList =
        this.getVolumes(
          item.Volumes || [],
          item.RaidController_ID,
          `/UI/Rest/System/Storage/${item.RaidController_ID}`,
          index,
          raidType,
          isSPType
        ) || [];
      const driveList =
        this.getDrivers(
          item.Drives || [],
          true,
          index,
          item.RaidController_ID,
          raidType
        ) || [];
      if (volumeList && volumeList.length > 0) {
        raid.addTreeNode(volumeList);
      }
      if (driveList && driveList.length > 0) {
        raid.addTreeNode(driveList);
      }
      raid.checkBtnState(
        driveList.length,
        volumeList.length,
        item.VolumeSupported
      );
      raidList.push(raid);
    });
    return raidList;
  }

  private getVolumes(
    volumes: any,
    raidId: number,
    raidUrl: string,
    raidIndex: number,
    raidType: string,
    isSPType: boolean
  ): VolumeNode[] {
    if (!volumes || volumes.length < 1) {
      return null;
    }
    volumes = volumes.sort(sortBy('Name'));
    const volumeList: VolumeNode[] = [];
    volumes.forEach((item: any) => {
      const labelId = `${raidId}_${item.Volumes_ID}`;
      const url = `/UI/Rest/System/Storage/${raidId}/Volumes/${item.Volumes_ID}`;
      const volume = new VolumeNode(
        item.Name,
        url,
        raidUrl,
        item.Volumes_ID,
        raidIndex,
        item.VolumeRaidLevel,
        labelId,
        raidType,
        isSPType,
        item.SSDCachecadeVolume,
        item.SSDCachingEnabled,
        item.CapacityBytes
      );
      const spansList = this.getSpans(
        item.Spans || [],
        url,
        item.Volumes_ID,
        raidIndex,
        labelId,
        raidType
      );
      const hotSpare = this.getHotSpares(
        item.HotSpares || [],
        item.Volumes_ID,
        raidIndex,
        labelId,
        raidType
      );
      const driversList = this.getDrivers(
        item.Drives || [],
        true,
        raidIndex,
        labelId,
        raidType
      );
      if (spansList && spansList.length > 0) {
        volume.addTreeNode(spansList);
      }
      if (hotSpare && hotSpare.length > 0) {
        volume.addTreeNode(hotSpare);
      }
      if (driversList && driversList.length > 0) {
        volume.addTreeNode(driversList);
      }
      volumeList.push(volume);
    });
    return volumeList;
  }

  private getSpans(
    spans: any,
    spanURL: string,
    volumeId: string,
    raidIndex: number,
    volumeLabel: string,
    raidType: string
  ): SpanNode[] {
    if (!spans || spans.length < 1) {
      return null;
    }
    spans = spans.sort(sortBy('Name'));
    const spanList: SpanNode[] = [];
    spans.forEach((item: any, index: number) => {
      const span = new SpanNode(
        item.Name,
        '',
        `${volumeId}Span${index}`,
        raidIndex,
        `${volumeLabel}_Span${index}`
      );
      const drivers = item.Drives;
      const children = this.getDrivers(
        drivers || [],
        true,
        raidIndex,
        volumeLabel,
        raidType
      );
      span.setChildren = children;
      spanList.push(span);
    });
    return spanList;
  }

  private getHotSpares(
    hotSpares: any,
    volumeId: string,
    raidIndex: number,
    volumeLabel: string,
    raidType: string
  ): HotSpareNode[] {
    if (!hotSpares || hotSpares.length < 1) {
      return null;
    }
    const hotSpareList: HotSpareNode[] = [];
    const hotSpare = new HotSpareNode(
      hotSpares.Name,
      `${volumeId}HotSpares`,
      `${volumeLabel}_HotSpares`
    );
    const drivers = hotSpares.Drives;
    const children = this.getDrivers(
      drivers || [],
      true,
      raidIndex,
      volumeLabel,
      raidType
    );
    hotSpare.setChildren = children;
    hotSpareList.push(hotSpare);
    return hotSpareList;
  }

  private getDrivers(
    drives: any,
    isRaid: boolean,
    raidIndex?: number,
    volumeLabel?: string,
    raidType?: string
  ): DriverNode[] {
    if (!drives || drives.length < 1) {
      return null;
    }
    drives = drives.sort(sortBy('Name'));
    const driverList: DriverNode[] = [];
    drives.forEach((item: any, index) => {
      const labelId = volumeLabel
        ? `${volumeLabel}_${item.Drives_ID}`
        : item.Drives_ID;
      const cryptoEraseSupported = item.CryptoEraseSupported ? true : false;
      const driver = new DriverNode(
        item.Name,
        `/UI/Rest/System/Storage/Drives/${item.Drives_ID}`,
        isRaid,
        cryptoEraseSupported,
        item.Drives_ID,
        raidIndex,
        labelId,
        raidType
      );
      driverList.push(driver);
    });
    return driverList;
  }

  private getData(url: string) {
    return this.http.get(url).pipe(map((data: any) => data.body));
  }

  public deleteVolumn(logicalID: string, raidId: string) {
    return this.http.delete(
      `/UI/Rest/System/Storage/${raidId}/Volumes/${logicalID}`
    );
  }

  public deleteBatchVolumn(nodeArr: TiTreeNode[]) {
    const post$ = [of('')];
    nodeArr.forEach((node) => {
      const volumeNode = node as VolumeNode;
      const raid = StorageTree.getInstance().getParentRaid(
        volumeNode.getRaidIndex
      );
      post$.push(this.deleteVolumn(node.id, raid.id));
    });
    return forkJoin(post$);
  }

  public entryDrive(driveId: string) {
    const url = `/UI/Rest/System/Storage/Drives/${driveId}/PhysicalDrivesCryptoErase`;
    return this.http.post(url, {});
  }
}
