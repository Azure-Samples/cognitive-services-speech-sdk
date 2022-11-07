// <copyright file="20221107110715_Init.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

#nullable disable

namespace Connector.Migrations
{
    using System;
    using Microsoft.EntityFrameworkCore.Migrations;

    public partial class Init : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            _ = migrationBuilder ?? throw new ArgumentNullException(nameof(migrationBuilder));

            migrationBuilder.CreateTable(
                name: "Transcriptions",
                columns: table => new
                {
                    ID = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Locale = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    Name = table.Column<string>(type: "nvarchar(500)", maxLength: 500, nullable: true),
                    Source = table.Column<string>(type: "nvarchar(500)", maxLength: 500, nullable: true),
                    Timestamp = table.Column<DateTime>(type: "datetime2", nullable: false),
                    Duration = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    DurationInSeconds = table.Column<double>(type: "float", nullable: false),
                    NumberOfChannels = table.Column<int>(type: "int", nullable: false),
                    ApproximateCost = table.Column<float>(type: "real", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Transcriptions", x => x.ID);
                });

            migrationBuilder.CreateTable(
                name: "CombinedRecognizedPhrases",
                columns: table => new
                {
                    ID = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Channel = table.Column<int>(type: "int", nullable: false),
                    Lexical = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Itn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    MaskedItn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Display = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    SentimentNegative = table.Column<double>(type: "float", nullable: false),
                    SentimentNeutral = table.Column<double>(type: "float", nullable: false),
                    SentimentPositive = table.Column<double>(type: "float", nullable: false),
                    TranscriptionID = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_CombinedRecognizedPhrases", x => x.ID);
                    table.ForeignKey(
                        name: "FK_CombinedRecognizedPhrases_Transcriptions_TranscriptionID",
                        column: x => x.TranscriptionID,
                        principalTable: "Transcriptions",
                        principalColumn: "ID");
                });

            migrationBuilder.CreateTable(
                name: "RecognizedPhrases",
                columns: table => new
                {
                    ID = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    RecognitionStatus = table.Column<string>(type: "nvarchar(32)", maxLength: 32, nullable: true),
                    Speaker = table.Column<int>(type: "int", nullable: false),
                    Channel = table.Column<int>(type: "int", nullable: false),
                    Offset = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    Duration = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    SilenceBetweenCurrentAndPreviousSegmentInMs = table.Column<int>(type: "int", nullable: false),
                    CombinedRecognizedPhraseID = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_RecognizedPhrases", x => x.ID);
                    table.ForeignKey(
                        name: "FK_RecognizedPhrases_CombinedRecognizedPhrases_CombinedRecognizedPhraseID",
                        column: x => x.CombinedRecognizedPhraseID,
                        principalTable: "CombinedRecognizedPhrases",
                        principalColumn: "ID");
                });

            migrationBuilder.CreateTable(
                name: "NBests",
                columns: table => new
                {
                    ID = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Confidence = table.Column<double>(type: "float", nullable: false),
                    Lexical = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Itn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    MaskedItn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Display = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    SentimentNegative = table.Column<double>(type: "float", nullable: false),
                    SentimentNeutral = table.Column<double>(type: "float", nullable: false),
                    SentimentPositive = table.Column<double>(type: "float", nullable: false),
                    RecognizedPhraseID = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_NBests", x => x.ID);
                    table.ForeignKey(
                        name: "FK_NBests_RecognizedPhrases_RecognizedPhraseID",
                        column: x => x.RecognizedPhraseID,
                        principalTable: "RecognizedPhrases",
                        principalColumn: "ID");
                });

            migrationBuilder.CreateTable(
                name: "Words",
                columns: table => new
                {
                    ID = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Word = table.Column<string>(type: "nvarchar(511)", maxLength: 511, nullable: true),
                    Offset = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    Duration = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    Confidence = table.Column<double>(type: "float", nullable: false),
                    NBestID = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Words", x => x.ID);
                    table.ForeignKey(
                        name: "FK_Words_NBests_NBestID",
                        column: x => x.NBestID,
                        principalTable: "NBests",
                        principalColumn: "ID");
                });

            migrationBuilder.CreateIndex(
                name: "IX_CombinedRecognizedPhrases_TranscriptionID",
                table: "CombinedRecognizedPhrases",
                column: "TranscriptionID");

            migrationBuilder.CreateIndex(
                name: "IX_NBests_RecognizedPhraseID",
                table: "NBests",
                column: "RecognizedPhraseID");

            migrationBuilder.CreateIndex(
                name: "IX_RecognizedPhrases_CombinedRecognizedPhraseID",
                table: "RecognizedPhrases",
                column: "CombinedRecognizedPhraseID");

            migrationBuilder.CreateIndex(
                name: "IX_Words_NBestID",
                table: "Words",
                column: "NBestID");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            _ = migrationBuilder ?? throw new ArgumentNullException(nameof(migrationBuilder));

            migrationBuilder.DropTable(
                name: "Words");

            migrationBuilder.DropTable(
                name: "NBests");

            migrationBuilder.DropTable(
                name: "RecognizedPhrases");

            migrationBuilder.DropTable(
                name: "CombinedRecognizedPhrases");

            migrationBuilder.DropTable(
                name: "Transcriptions");
        }
    }
}
