import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { IInfo, IUpdateBody } from '../product.datatype';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';

@Injectable({
    providedIn: 'root'
})
export class ProductService {

    constructor(
        private http: HttpService,
    ) { }

    public getData() {
        return this.getProductInfo();
    }

    private initMainBoardValue(value: any) {
        const noDataStr = '--';
        if (value === null) {
            return noDataStr;
        } else {
            return value;
        }
    }
    private factoryProductInfo(productData): IInfo {
        const {
            Product: {
                ProductName: productName,
                ProductAlias: productAlias,
                ProductManufacturer: productManufacturer,
                AssetTag: assetTag,
                ProductSN: productSN,
                SystemSN: systemSN,
                DeviceLocation: deviceLocation,
                PartNumber: bomNumber
            },
            MainBoard: {
                BMCVersion,
                BIOSVersion,
                CPLDVersion,
                ActiveUbootVersion: activeUbootVersion,
                BackupUbootVersion: backupUbootVersion,
                PCHModel,
                PCBVersion,
                BoardID: boardID,
                BoardSN: boardSN,
                BoardModel: boardModel,
                BoardManufacturer: boardManufacturer,
                PartNumber: partNumber,
                ChassisID: chassisID,
                DeviceSlotID: deviceSlotID,
                ProductId: productId
            },
            DigitalWarranty: {
                ProductName: productNamePolicy,
                SerialNumber: serialNumber,
                StartPoint: startPoint,
                UnitType: unitType,
                UUID,
                ManufactureDate: manufactureDate,
                Lifespan: lifespan,
            },
            OS: showSmsEnabled
        } = productData;

        const noDataStr = '--';
        let name = productName;
        if (productAlias) {
            name += `(${productAlias})`;
        }
        let resultData: IInfo = {
            productInfoName: this.initMainBoardValue(name),
            productSerialNum: this.initMainBoardValue(productSN),
            systemSN: this.initMainBoardValue(systemSN),
            pchVersion: this.initMainBoardValue(PCHModel),
            showSmsEnabled: Boolean(showSmsEnabled) && showSmsEnabled.iBMARunningStatus === 'Running',
            assetTag,
            deviceSlotID: this.initMainBoardValue(deviceSlotID),
            biosVersion: this.initMainBoardValue(BIOSVersion),
            boardVersion: this.initMainBoardValue(boardModel),
            pcbVersion: this.initMainBoardValue(PCBVersion),
            boardId: this.initMainBoardValue(boardID),
            cpldVersion: this.initMainBoardValue(CPLDVersion),
            serial: this.initMainBoardValue(boardSN),
            backupBoot: this.initMainBoardValue(backupUbootVersion),
            boardManu: this.initMainBoardValue(boardManufacturer),
            chasisId: this.initMainBoardValue(chassisID),
            mainBoot: this.initMainBoardValue(activeUbootVersion),
            ibmcVersion: this.initMainBoardValue(BMCVersion),
            position: deviceLocation,
            productManufacturer: this.initMainBoardValue(productManufacturer),
            productNamePolicy: this.initMainBoardValue(productNamePolicy),
            serialNumber: this.initMainBoardValue(serialNumber),
            startPoint: this.initMainBoardValue(startPoint),
            unitType: this.initMainBoardValue(unitType),
            partNumber: this.initMainBoardValue(partNumber),
            UUID: this.initMainBoardValue(UUID),
            manufactureDate: this.initMainBoardValue(manufactureDate),
            productId: this.initMainBoardValue(productId),
            lifespan: {
                hide: !lifespan,
                value: lifespan
            },
            bomNumber: this.initMainBoardValue(bomNumber)
        };

        if (showSmsEnabled) {
            const {
                Domain: domain,
                HostName: hostName,
                HostDescription: hostDescription,
                OSVersion,
                KernalVersion: kernalVersion,
                iBMAVersion,
                iBMARunningStatus,
                iBMADriver,
            } = showSmsEnabled;
            resultData = {
                ...resultData,
                localDomain: this.initMainBoardValue(hostName),
                domain: this.initMainBoardValue(domain),
                description: this.initMainBoardValue(hostDescription),
                kernalVersion: this.initMainBoardValue(kernalVersion),
                operator: this.initMainBoardValue(OSVersion),
                ibmaService: this.initMainBoardValue(iBMAVersion),
                ibmaStatus: this.initMainBoardValue(iBMARunningStatus),
                showSysInfo: iBMARunningStatus === 'Running',
                ibmaDriver: this.initMainBoardValue(iBMADriver),
            };
        }

        return resultData;
    }

    public updateAssetTag(value: string) {
        return this.updateProductInfo({
            AssetTag: value
        });
    }

    public updateDeviceLocation(value: string) {
        return this.updateProductInfo({
            DeviceLocation: value
        });
    }

    // 电子保单日期保存
    public updateWarrantyDate(value: string) {
        return this.updateProductInfo({
            DigitalWarranty: {
                'StartPoint': value
            }
        });
    }
    // 电子保单年限保存
    public updateWarrantySpan(value: string) {
        return this.updateProductInfo({
            DigitalWarranty: {
                'Lifespan': parseInt(value, 10)
            }
        });
    }

    private getProductInfo(): Observable<IInfo> {
        const url = '/UI/Rest/System/ProductInfo';
        return this.http.get(url).pipe(
            pluck('body'),
            map((data) => this.factoryProductInfo(data))
        );
    }

    private updateProductInfo(value: IUpdateBody) {
        const url = '/UI/Rest/System/ProductInfo';
        return this.http.patch(url, value);
    }

}
