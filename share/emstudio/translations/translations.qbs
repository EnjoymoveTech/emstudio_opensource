import qbs 1.0

Product {
    name: "Translations"
    type: "qm"
    Depends { name: "Qt.core" }
    Depends { name: "qtc" }

    Group {
        files: ["*.ts"]
        excludeFiles: [
            "emstudio_es.ts",
            "emstudio_hu.ts",
            "emstudio_it.ts",
        ]
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: qtc.ide_data_path + "/translations"
    }
}
